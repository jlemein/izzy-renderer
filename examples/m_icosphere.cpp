#include <ecs_firstpersoncontrol.h>
#include <ecs_light.h>
#include <ecs_wireframe.h>
#include <geo_primitivefactory.h>
#include <gui_window.h>
#include <izz_izzy.h>
#include <izz_resourcemanager.h>
#include <wsp_workspace.h>
#include <cxxopts.hpp>
#include <iostream>
#include "izz_entityfactory.h"
#include "izzgl_materialreader.h"
#include "izzgl_materialsystem.h"
#include <geo_primitivefactory.h>
#include <geo_mesh.h>

using namespace std;
using namespace izz;
using namespace geo;
using namespace glm;

namespace {
  std::shared_ptr<Izzy> izzy {nullptr};
}

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
  auto programArguments = parseProgramArguments(argc, argv);
  if (programArguments->debugMode) {
    spdlog::set_level(spdlog::level::debug);
  }

  try {
    izzy = Izzy::CreateSystems();

    auto window = make_shared<gui::Window>(izzy->getRegistry(), izzy->renderSystem, izzy->guiSystem);  // guiSystem);
    window->setWindowSize(1920, 1080);
    window->setTitle(fmt::format("Izzy Renderer: {}", programArguments->sceneFile.filename().string()));
    window->initialize();

    // setup camera
    auto camera = izzy->entityFactory->makeCamera("DummyCamera", 4);
    camera.add<ecs::FirstPersonControl>().onlyRotateOnMousePress = true;

    if (programArguments->materialsFile.empty()) {
      spdlog::warn("No materials provided. Rendering results may be different than expected.");
    } else {
      izz::gl::MaterialReader reader(izzy->materialSystem);
      reader.readMaterials(programArguments->materialsFile);
    }

    //    auto box = sceneGraph->addGeometry(PrimitiveFactory::MakeBox(), resourceManager->createMaterial("Wireframe"));
    auto box = izzy->entityFactory->addGeometry(izz::geo::PrimitiveFactory::MakeUVSphere("UvSphere"), izzy->materialSystem->createMaterial("Wireframe").id);
    box.add<ecs::Wireframe>();

    izzy->renderSystem->init(window->getDisplayDetails().windowWidth, window->getDisplayDetails().windowHeight);

    window->run();

  } catch (runtime_error& e) {
    spdlog::error(e.what());
    return EXIT_FAILURE;
  }
  return 0;
}