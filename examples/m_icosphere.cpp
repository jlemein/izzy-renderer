#include <core_util.h>
#include <ecs_firstpersoncontrol.h>
#include <ecs_light.h>
#include <ecs_wireframe.h>
#include <ecsg_scenegraph.h>
#include <geo_primitivefactory.h>
#include <izz_materialsystem.h>
#include <izz_resourcemanager.h>
#include <gui_window.h>
#include <wsp_workspace.h>
#include <cxxopts.hpp>
#include <iostream>

using namespace std;
using namespace lsw;
using namespace geo;
using lsw::core::Util;
using namespace glm;

std::shared_ptr<wsp::Workspace> parseProgramArguments(int argc, char* argv[]) {
  const std::string PROGRAM_NAME = "parallax";
  const std::string VERSION_STR = "1.0";
  cxxopts::Options _options(PROGRAM_NAME, "Icosphere Example.\n");
  _options.add_options()("m,materials", "Reference to a materials json file", cxxopts::value<std::string>())("h,help", "Print usage");
  auto result = _options.parse(argc, argv);

  if (result.count("help")) {
    std::cout << _options.help() << std::endl;
    exit(EXIT_SUCCESS);
  }

  if (result.count("version")) {
    std::cout << PROGRAM_NAME << " v" << VERSION_STR << std::endl;  // VERSION_STR is defined in CmakeLists.txt
    exit(EXIT_SUCCESS);
  }

  if (!result.count("materials")) {
    spdlog::error("Missing required command line argument --materials.");
    std::cout << _options.help() << std::endl;
    exit(EXIT_FAILURE);
  }

  auto workspace = wsp::WorkspaceManager::GetActiveWorkspace();
  workspace->materialsFile = result["materials"].as<std::string>();
//  workspace->path = workspace->materialsFile.parent_path();

  return wsp::WorkspaceManager::GetActiveWorkspace();
}

int main(int argc, char* argv[]) {
  cout << "Hello Icosphere" << endl;
  auto workspace = parseProgramArguments(argc, argv);

  try {
    auto resourceManager = make_shared<ResourceManager>();
    auto sceneGraph = make_shared<ecsg::SceneGraph>();
    auto materialSystem = make_shared<MaterialSystem>(sceneGraph, resourceManager);
    materialSystem->loadMaterialsFromFile(workspace->materialsFile);
    resourceManager->setMaterialSystem(materialSystem);

    auto renderSystem = make_shared<glrs::RenderSystem>(sceneGraph, static_pointer_cast<glrs::IMaterialSystem>(materialSystem));

    //    auto box = sceneGraph->addGeometry(PrimitiveFactory::MakeBox(), resourceManager->createMaterial("Wireframe"));
    auto box = sceneGraph->addGeometry(PrimitiveFactory::MakeUVSphere("UvSphere"), *resourceManager->getMaterialSystem()->createMaterial("Wireframe"));
    box.add<ecs::Wireframe>();

    auto viewer = make_shared<izz::gui::Window>(sceneGraph, renderSystem /*, guiSystem*/);

    // ==== CAMERA SETUP ====================================================
    auto camera = sceneGraph->makeCamera("DummyCamera", 4);
    camera.add<ecs::FirstPersonControl>().onlyRotateOnMousePress = true;
    viewer->setActiveCamera(camera);

    // ==== UI SETUP ========================================================
    viewer->setWindowSize(1024, 768);
    viewer->setTitle("Icosphere Wireframe");
    viewer->initialize();
    viewer->run();
  } catch (runtime_error& e) {
    spdlog::error(e.what());
    return EXIT_FAILURE;
  }
  return 0;
}