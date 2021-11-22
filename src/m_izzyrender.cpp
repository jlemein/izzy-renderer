//
// Created by jlemein on 11-03-21.
//

#include <anim_localrotation.h>
#include <core_util.h>
#include <ecs_firstpersoncontrol.h>
#include <ecs_light.h>
#include <ecs_transformutil.h>
#include <ecsg_scenegraph.h>
#include <geo_meshutil.h>
#include <geo_scene.h>
#include <georm_exrloader.h>
#include <georm_fontsystem.h>
#include <georm_materialsystem.h>
#include <georm_resourcemanager.h>
#include <georm_sceneloader.h>
#include <georm_stbtextureloader.h>
#include <georm_texturesystem.h>
#include <gui_system.h>
#include <vwr_viewer.h>
#include <wsp_workspace.h>
#include "gui_lighteditor.h"

#include <spdlog/spdlog.h>
#include <cxxopts.hpp>
#include <memory>
using namespace std;
using namespace lsw;
using namespace geo;
using lsw::core::Util;
using namespace glm;
using lsw::wsp::Workspace;

std::shared_ptr<Workspace> parseProgramArguments(int argc, char* argv[]);

int main(int argc, char* argv[]) {
  auto workspace = parseProgramArguments(argc, argv);
  if (workspace->debugMode) {
    spdlog::set_level(spdlog::level::info);
  }

  try {
    auto resourceManager = make_shared<georm::ResourceManager>();

    auto fontSystem = make_shared<georm::FontSystem>();
    auto sceneGraph = make_shared<ecsg::SceneGraph>();

    auto textureSystem = make_shared<georm::TextureSystem>();
    textureSystem->setTextureLoader(".exr", std::make_unique<georm::ExrLoader>());
    textureSystem->setTextureLoader(ExtensionList{".jpg", ".png", ".bmp"}, std::make_unique<georm::StbTextureLoader>());
    resourceManager->setTextureSystem(textureSystem);

    auto materialSystem = make_shared<georm::MaterialSystem>(sceneGraph, resourceManager);
    resourceManager->setMaterialSystem(materialSystem);

    auto sceneLoader = make_shared<georm::SceneLoader>(textureSystem, materialSystem);
    resourceManager->setSceneLoader(sceneLoader);

    if (workspace->materialsFile.empty()) {
      spdlog::warn("No materials provided. Rendering results may be different than expected.");
    } else {
      materialSystem->loadMaterialsFromFile(workspace->materialsFile);
    }

    auto renderSystem = make_shared<glrs::RenderSystem>(sceneGraph, static_pointer_cast<glrs::IMaterialSystem>(materialSystem));

    // ==== GUI =============================================================
    auto editor = make_shared<gui::GuiLightEditor>(sceneGraph, fontSystem);
    auto resourceInspector = make_shared<gui::ResourceInspector>(resourceManager);
    auto guiSystem = make_shared<GuiSystem>(vector<std::shared_ptr<IGuiWindow>>{editor, resourceInspector});
    auto viewer = make_shared<viewer::Viewer>(sceneGraph, renderSystem, resourceManager, nullptr);//guiSystem);

    // ==== SCENE SETUP ======================================================
    auto scene = resourceManager->getSceneLoader()->loadScene(workspace->sceneFile);
    ecs::TransformUtil::Scale(scene->rootNode()->transform, .20);

    for (auto& mesh : scene->m_meshes) {
<<<<<<< Updated upstream
      MeshUtil::ConvertToSmoothNormals(*mesh); // doesnt work yet, buggy
=======
>>>>>>> Stashed changes
      geo::MeshUtil::GenerateTangents(*mesh);
      MeshUtil::ConvertToSmoothNormals(*mesh); // doesnt work yet, buggy
    }

    sceneGraph->makeScene(*scene, ecsg::SceneLoaderFlags::All());

    // ==== LIGHTS SETUP ====================================================
    sceneGraph->makeDirectionalLight("Sun", glm::vec3(0.F, 1.0F, 1.0F));
//    auto ptLight = sceneGraph->makePointLight("PointLight", glm::vec3(1.F, 1.0F, -1.0F));
//    ptLight.get<ecs::PointLight>().intensity = 4.0F;

    // ==== CAMERA SETUP ====================================================
    auto camera = sceneGraph->makeCamera("DummyCamera", 4);
    camera.add<ecs::FirstPersonControl>().onlyRotateOnMousePress = true;
    viewer->setActiveCamera(camera);

    // ==== UI SETUP ========================================================
//    viewer->registerExtension(guiSystem);
    viewer->setWindowSize(1024, 768);
    viewer->setTitle(fmt::format("Izzy Renderer: {}", workspace->sceneFile.filename().string()));
    viewer->initialize();
    viewer->run();
  } catch (runtime_error& e) {
    spdlog::error(e.what());
    spdlog::error("Aborting the application with exit code ({})", EXIT_FAILURE);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

namespace {
#ifndef NDEBUG
const char* DEBUG_MODE = "true";
#else
const char* DEBUG_MODE = "false";
#endif  // NDEBUG
}  // namespace

std::shared_ptr<Workspace> parseProgramArguments(int argc, char* argv[]) {
  const std::string PROGRAM_NAME = "wera3d";
  cxxopts::Options _options(PROGRAM_NAME, "Wera3d renderer.\nCopyright reserved to jlemein.nl\n");
  _options.add_options()("d,debug", "Enable debug mode", cxxopts::value<bool>()->default_value(DEBUG_MODE))(
      "s,scene", "A scene file for visualization (*.fbx, *.obj)", cxxopts::value<std::string>())("m,materials", "Reference to a materials json file",
                                                                                                 cxxopts::value<std::string>())(
      "w,workspace", "Workspace directory", cxxopts::value<std::string>())("v,version", "Version information")("h,help", "Print usage");
  auto result = _options.parse(argc, argv);

  if (result.count("help")) {
    std::cout << _options.help() << std::endl;
    exit(EXIT_SUCCESS);
  }

  if (result.count("version")) {
    std::cout << PROGRAM_NAME << " v" << VERSION_STR << std::endl;  // VERSION_STR is defined in CmakeLists.txt
    exit(EXIT_SUCCESS);
  }

  if (!result.count("scene")) {
    spdlog::error("Missing required command line argument --scene.");
    std::cout << _options.help() << std::endl;
    exit(EXIT_FAILURE);
  }

  auto workspace = wsp::WorkspaceManager::GetActiveWorkspace();
  workspace->sceneFile = result["scene"].as<std::string>();
  workspace->path = workspace->sceneFile.parent_path();
  workspace->materialsFile = result["materials"].as<std::string>();

  const char* weraHomeEnv = getenv("WERA3D_HOME");
  if (weraHomeEnv) {
    workspace->lsw_home_directory = weraHomeEnv;
    workspace->path = argv[0];
  } else {
    workspace->isStrictMode = true;  // no default materials to read from, which implies strict mode.
  }

  workspace->debugMode = result["debug"].as<bool>();

  cout << "Scene file: " << workspace->sceneFile << endl;
  cout << "Materials file: " << (workspace->materialsFile.empty() ? "<not specified>" : workspace->materialsFile) << endl;
  cout << "Strict mode: " << (workspace->isStrictMode ? "enabled" : "disabled") << endl;

  return workspace;
}