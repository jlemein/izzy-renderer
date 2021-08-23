//
// Created by jlemein on 11-03-21.
//
#include <georm_materialsystem.h>
#include <memory>
#include <spdlog/spdlog.h>
#include <vwr_viewer.h>
//#include <res_resourcemanager.h>
#include <anim_localrotation.h>
#include <core_util.h>
#include <ecs_firstpersoncontrol.h>
#include <ecs_transformutil.h>
#include <ecsg_scenegraph.h>
#include <geo_primitivefactory.h>
#include <georm_materialsystem.h>
#include <georm_resourcemanager.h>
#include <gui_system.h>
#include <gui_lighteditor.h>
#include <georm_fontsystem.h>
#include <ecs_light.h>
#include <cxxopts.hpp>

using namespace std;
using namespace lsw;
using namespace geo;
using lsw::core::Util;
using namespace glm;

int main(int argc, char* argv[]) {
#ifndef NDEBUG
  spdlog::set_level(spdlog::level::debug);
#endif  // NDEBUG

  cxxopts::Options options("wera3d", "Wera3d renderer is a hybrid between rasterized rendering and ray tracing.");
  options.add_options()
    ("d,debug", "Enable debugging")
    ("s,scene", "A scene file for visualization (*.fbx, *.obj)", cxxopts::value<std::string>())
    ("m,materials", "Reference to a materials json file", cxxopts::value<std::string>())
    ("h,help", "Print usage");
  auto result = options.parse(argc, argv);

  if (result.count("help"))
  {
    std::cout << options.help() << std::endl;
    exit(0);
  }

  auto workspaceDir = std::getenv("WERA3D_HOME");
  std::string sWorkspaceDir = "";
  if (workspaceDir) {
    sWorkspaceDir = workspaceDir;
  }

  std::string sceneFile {""}, materialsFile {""};
  try {
    sceneFile = result["scene"].as<std::string>();
    materialsFile = result["materials"].as<std::string>();
  } catch (std::domain_error e) {
    spdlog::error("Missing input arguments");
    std::cout << options.help() << std::endl;
    return EXIT_FAILURE;
  }

  try {
    auto resourceManager = make_shared<georm::ResourceManager>();
    auto fontSystem = make_shared<georm::FontSystem>(workspaceDir);
    auto sceneGraph = make_shared<ecsg::SceneGraph>();
    auto materialSystem = make_shared<georm::MaterialSystem>(sceneGraph, resourceManager);
    materialSystem->loadMaterialsFromFile(materialsFile);
    resourceManager->setMaterialSystem(materialSystem);

    auto renderSystem = make_shared<ecs::RenderSystem>(sceneGraph, static_pointer_cast<ecs::IMaterialSystem>(materialSystem));
    auto editor = make_shared<gui::GuiLightEditor>(sceneGraph, fontSystem);
    auto guiSystem = make_shared<GuiSystem>(editor);
    auto viewer = make_shared<viewer::Viewer>(sceneGraph, renderSystem, resourceManager->getRawResourceManager(), guiSystem);

    // ==== SCENE SETUP ======================================================
    spdlog::info("Loading scene file: {}", sceneFile);

    // ==== LIGHTS SETUP ====================================================
    sceneGraph->makeDirectionalLight("Sun", glm::vec3(0.F, 1.0F, 1.0F));

    // ==== CAMERA SETUP ====================================================
    auto camera = sceneGraph->makeCamera("DummyCamera", 4);
    camera.add<ecs::FirstPersonControl>().onlyRotateOnMousePress = true;
    viewer->setActiveCamera(camera);

    // ==== UI SETUP ========================================================
    viewer->registerExtension(guiSystem);

    viewer->setWindowSize(1024, 768);
    viewer->setTitle("Normal mapping");
    viewer->initialize();
    viewer->run();
  } catch (runtime_error& e) {
    spdlog::error(e.what());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}