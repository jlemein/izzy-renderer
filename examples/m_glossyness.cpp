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
    spdlog::set_level(spdlog::level::info);
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

    if (workspace->materialsFile.empty()) {
      spdlog::warn("No materials provided. Rendering results may be different than expected.");
    } else {
      materialSystem->loadMaterialsFromFile(workspace->materialsFile);
    }

    sceneGraph->setDefaultMaterial(materialSystem->createMaterial("BlinnPhongSimple"));

    auto renderSystem = make_shared<glrs::RenderSystem>(sceneGraph, static_pointer_cast<glrs::IMaterialSystem>(materialSystem));
    renderSystem->getLightSystem().setDefaultPointLightMaterial(materialSystem->createMaterial("pointlight"));

    // ==== GUI =============================================================
    auto editor = make_shared<gui::LightEditor>(sceneGraph, fontSystem);
    auto guiSystem = make_shared<gui::GuiSystem>(fontSystem, vector<std::shared_ptr<gui::IGuiWindow>>{editor});
    auto viewer = make_shared<gui::Window>(sceneGraph, renderSystem, guiSystem);  // guiSystem);

    // ==== SCENE SETUP ======================================================
    auto sphere11 = sceneGraph->makeMesh(PrimitiveFactory::MakeUVSphere("Sphere11", .5F));
    ecs::TransformUtil::SetPosition(sphere11.get<ecs::Transform>(), glm::vec3(-1.25F, .75F, 0.0F));

    auto sphere12 = sceneGraph->makeMesh(PrimitiveFactory::MakeUVSphere("Sphere12", .5F));
    ecs::TransformUtil::SetPosition(sphere12.get<ecs::Transform>(), glm::vec3(.0F, .75F, 0.0F));

    auto sphere13 = sceneGraph->makeMesh(PrimitiveFactory::MakeUVSphere("Sphere13", .5F));
    ecs::TransformUtil::SetPosition(sphere13.get<ecs::Transform>(), glm::vec3(1.25F, .75F, 0.0F));

    auto sphere21 = sceneGraph->makeMesh(PrimitiveFactory::MakeUVSphere("Sphere21", .5F));
    ecs::TransformUtil::SetPosition(sphere21.get<ecs::Transform>(), glm::vec3(-1.25F, -.75F, 0.0F));

    auto sphere22 = sceneGraph->makeMesh(PrimitiveFactory::MakeUVSphere("Sphere22", .5F));
    ecs::TransformUtil::SetPosition(sphere22.get<ecs::Transform>(), glm::vec3(.0F, -.75F, 0.0F));

    auto sphere23 = sceneGraph->makeMesh(PrimitiveFactory::MakeUVSphere("Sphere23", .5F));
    ecs::TransformUtil::SetPosition(sphere23.get<ecs::Transform>(), glm::vec3(1.25F, -.75F, 0.0F));


    // ==== LIGHTS SETUP ====================================================
    sceneGraph->makeDirectionalLight("Sun", glm::vec3(0.F, 1.0F, 1.0F));
    auto ptLight = sceneGraph->makePointLight("PointLight", glm::vec3(1.F, 1.0F, -1.0F));
    auto& lightComp = ptLight.get<ecs::PointLight>();
    lightComp.intensity = 4.0;
    lightComp.color = glm::vec3(0.1, 0.1, 1.0);
    ptLight.add(PrimitiveFactory::MakeUVSphere("SphericalPointLight", 0.1));

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
  const std::string PROGRAM_NAME = "glossyness";
  cxxopts::Options _options(PROGRAM_NAME, "Glossyness example.\n");
  _options.add_options()
      ("d,debug", "Enable debug mode", cxxopts::value<bool>()->default_value(DEBUG_MODE))
      ("m,materials", "Reference to a materials json file", cxxopts::value<std::string>()->default_value(""))
      ("v,version", "Version information")
      ("h,help", "Print usage");
  auto result = _options.parse(argc, argv);

  if (result.count("help")) {
    std::cout << _options.help() << std::endl;
    exit(EXIT_SUCCESS);
  }

  if (result.count("version")) {
    std::cout << PROGRAM_NAME << " v" << VERSION_STR << std::endl;  // VERSION_STR is defined in CmakeLists.txt
    exit(EXIT_SUCCESS);
  }

  auto workspace = wsp::WorkspaceManager::GetActiveWorkspace();
  workspace->materialsFile = result["materials"].as<std::string>();
  workspace->debugMode = result["debug"].as<bool>();

  cout << "Materials file: " << (workspace->materialsFile.empty() ? "<not specified>" : workspace->materialsFile) << endl;
  cout << "Strict mode: " << (workspace->isStrictMode ? "enabled" : "disabled") << endl;

  return workspace;
}