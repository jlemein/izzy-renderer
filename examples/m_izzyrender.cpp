//
// Created by jlemein on 11-03-21.
//

#include "anim_localrotation.h"
#include "core_util.h"
#include "ecs_firstpersoncontrol.h"
#include "ecs_light.h"
#include "ecs_transformutil.h"
#include "ecsg_scenegraph.h"
#include "geo_meshutil.h"
#include "geo_scene.h"
#include "izz_exrloader.h"
#include "izz_fontsystem.h"
#include "izz_materialsystem.h"
#include "izz_resourcemanager.h"
#include "izz_sceneloader.h"
#include "izz_stbtextureloader.h"
#include "izz_texturesystem.h"
#include "gui_lighteditor.h"
#include "vwr_viewer.h"
#include "wsp_workspace.h"
#include "gui_guiwindow.h"

#include "uniform_blinnphongsimple.h"

#include "geo_primitivefactory.h"
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
    auto resourceManager = make_shared<ResourceManager>();
    auto sceneGraph = make_shared<ecsg::SceneGraph>();

    auto textureSystem = make_shared<TextureSystem>();
    textureSystem->setTextureLoader(".exr", std::make_unique<ExrLoader>(true));
    textureSystem->setTextureLoader(ExtensionList{".jpg", ".png", ".bmp"}, std::make_unique<StbTextureLoader>(true));
    resourceManager->setTextureSystem(textureSystem);

    auto materialSystem = make_shared<MaterialSystem>(sceneGraph, resourceManager);
    resourceManager->setMaterialSystem(materialSystem);

    auto sceneLoader = make_shared<SceneLoader>(textureSystem, materialSystem);
    resourceManager->setSceneLoader(sceneLoader);

    if (workspace->materialsFile.empty()) {
      spdlog::warn("No materials provided. Rendering results may be different than expected.");
    } else {
      materialSystem->loadMaterialsFromFile(workspace->materialsFile);
    }

    auto renderSystem = make_shared<glrs::RenderSystem>(sceneGraph, materialSystem);
    renderSystem->getLightSystem().setDefaultPointLightMaterial(materialSystem->createMaterial("pointlight"));

    // ==== GUI =============================================================
    auto fontSystem = make_shared<FontSystem>();
    auto editor = make_shared<gui::GuiLightEditor>(sceneGraph, fontSystem);
    auto guiSystem = make_shared<gui::GuiSystem>(vector<std::shared_ptr<gui::IGuiWindow>>{editor});
    auto viewer = make_shared<viewer::Viewer>(sceneGraph, renderSystem, resourceManager, guiSystem);  // guiSystem);

    // ==== SCENE SETUP ======================================================
    auto scene = resourceManager->getSceneLoader()->loadScene(workspace->sceneFile);
    ecs::TransformUtil::Scale(scene->rootNode()->transform, .20);
    for (auto& mesh : scene->m_meshes) {
      geo::MeshUtil::GenerateTangentsAndBitangentsFromUvCoords(*mesh);
      MeshUtil::GenerateSmoothNormals(*mesh);
    }
    sceneGraph->makeScene(*scene, ecsg::SceneLoaderFlags::All());

    // add a plane
    auto plane = PrimitiveFactory::MakePlane("Plane", 25.0, 25.0);
    MeshUtil::ScaleUvCoords(plane, 3, 3);
    auto tableCloth = materialSystem->createMaterial("table_cloth");
//    blinn->userProperties.setValue("albedo", glm::vec4(0.3, 0.7, 0.2, 0.0));
//    blinn->userProperties.setValue("specular", glm::vec4(0.1, 0.1, 0.1, 0.0));
//    blinn->userProperties.setFloat("shininess", 32.0);
    sceneGraph->addGeometry(plane, tableCloth);

    // ==== LIGHTS SETUP ====================================================
    sceneGraph->makeDirectionalLight("Sun", glm::vec3(0.F, 1.0F, 1.0F));
    auto ptLight1 = sceneGraph->makePointLight("PointLight 1", glm::vec3(1.F, 1.0F, -1.0F));
    auto& lightComp = ptLight1.get<ecs::PointLight>();
    lightComp.intensity = 1.0;
    lightComp.color = glm::vec3(1.0, 1.0, 1.0);
    ptLight1.add(geo::PrimitiveFactory::MakeUVSphere("SphericalPointLight", 0.1));

    auto ptLight2 = sceneGraph->makePointLight("PointLight 2", glm::vec3(-10.F, 1.0F, -1.0F));
    ptLight2.get<ecs::PointLight>().intensity = 1.4;
    ptLight2.add(geo::PrimitiveFactory::MakeUVSphere("SphericalPointLight", 0.1));

    // ==== CAMERA SETUP ====================================================
    auto camera = sceneGraph->makeCamera("DummyCamera", 4);
    camera.add<ecs::FirstPersonControl>().onlyRotateOnMousePress = true;
    viewer->setActiveCamera(camera);

    // ==== UI SETUP ========================================================

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
  const std::string PROGRAM_NAME = "izzyrender";
  cxxopts::Options _options(PROGRAM_NAME, "Wera3d renderer.\n");
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
  workspace->materialsFile = result["materials"].as<std::string>();
  workspace->debugMode = result["debug"].as<bool>();

  cout << "Scene file: " << workspace->sceneFile << endl;
  cout << "Materials file: " << (workspace->materialsFile.empty() ? "<not specified>" : workspace->materialsFile) << endl;
  cout << "Strict mode: " << (workspace->isStrictMode ? "enabled" : "disabled") << endl;

  return workspace;
}