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
#include <izz_exrloader.h>
#include <izz_fontsystem.h>
#include <izz_materialsystem.h>
#include <izz_resourcemanager.h>
#include <izz_sceneloader.h>
#include <izz_stbtextureloader.h>
#include <izz_texturesystem.h>
#include <gui_lighteditor.h>
#include <gui_window.h>
#include <wsp_workspace.h>
#include "gui_iguiwindow.h"

#include <geo_primitivefactory.h>
#include <spdlog/spdlog.h>
#include <cxxopts.hpp>
#include <memory>
using namespace std;
using namespace lsw;
using namespace izz;
using namespace lsw::geo;
using lsw::core::Util;
using namespace glm;
using lsw::wsp::Workspace;

std::shared_ptr<Workspace> parseProgramArguments(int argc, char* argv[]);

int main(int argc, char* argv[]) {
  auto workspace = parseProgramArguments(argc, argv);
  if (workspace->debugMode) {
    spdlog::set_level(spdlog::level::debug);
  }

  try {
    auto resourceManager = make_shared<ResourceManager>();
    auto fontSystem = make_shared<FontSystem>();
    auto sceneGraph = make_shared<ecsg::SceneGraph>();
    auto textureSystem = make_shared<TextureSystem>();
    textureSystem->setTextureLoader(".exr", std::make_unique<ExrLoader>(true));
    textureSystem->setTextureLoader(ExtensionList{".jpg", ".png", ".bmp"}, std::make_unique<StbTextureLoader>(true));
    resourceManager->setTextureSystem(textureSystem);

    auto materialSystem = make_shared<MaterialSystem>(sceneGraph, resourceManager);
    resourceManager->setMaterialSystem(materialSystem);

    auto sceneLoader = make_shared<SceneLoader>(textureSystem, materialSystem);
    resourceManager->setSceneLoader(sceneLoader);

    auto renderSystem = make_shared<glrs::RenderSystem>(sceneGraph, static_pointer_cast<glrs::IMaterialSystem>(materialSystem));

    // ==== GUI =============================================================
    auto editor = make_shared<gui::LightEditor>(sceneGraph, fontSystem);
    auto guiSystem = make_shared<gui::GuiSystem>(fontSystem, vector<std::shared_ptr<gui::IGuiWindow>>{editor});
    auto viewer = make_shared<gui::Window>(sceneGraph, renderSystem, guiSystem);

    // ==== SCENE SETUP ======================================================
    auto uvPlane = PrimitiveFactory::MakePlane("UVPlane", 10, 10);
    auto mat = materialSystem->createMaterial("BlinnPhong");
    std::cout << "Number of textures: " << mat->textures.size() << " - " << mat->texturePaths.size() << std::endl;
    for(auto& t : mat->textures) {
      std::cout << t.first << " " << t.second->path << std::endl;
    }
    sceneGraph->addGeometry(uvPlane, mat);


    // ==== LIGHTS SETUP ====================================================
//    sceneGraph->makeDirectionalLight("Sun", glm::vec3(0.F, 1.0F, 1.0F));
    sceneGraph->makePointLight("PointLight", glm::vec3(0.F, .01F, .0F), {.intensity = 1.0, .radius=0.2F, .color=glm::vec3{1.0,0.5, 0.0}, });

    // ==== CAMERA SETUP ====================================================
    auto camera = sceneGraph->makeCamera("DummyCamera", 4);
    camera.add<ecs::FirstPersonControl>().onlyRotateOnMousePress = true;

    viewer->setActiveCamera(camera);
    viewer->setWindowSize(1024, 768);
    viewer->setTitle("Normal mapping");

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

  if (!result.count("scene")) {
    spdlog::error("Missing required command line argument --scene.");
    std::cout << _options.help() << std::endl;
    exit(EXIT_FAILURE);
  }

  auto workspace = wsp::WorkspaceManager::GetActiveWorkspace();
  workspace->sceneFile = result["scene"].as<std::string>();
//  workspace->path = workspace->sceneFile.parent_path();
  workspace->materialsFile = result["materials"].as<std::string>();

  workspace->debugMode = result["debug"].as<bool>();

  cout << "Scene file: " << workspace->sceneFile << endl;
  cout << "Materials file: " << (workspace->materialsFile.empty() ? "<not specified>" : workspace->materialsFile) << endl;
  cout << "Strict mode: " << (workspace->isStrictMode ? "enabled" : "disabled") << endl;

  return workspace;
}