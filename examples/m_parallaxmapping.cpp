//
// Created by jlemein on 11-03-21.
//
#include <izz_materialsystem.h>
#include <spdlog/spdlog.h>
#include <vwr_viewer.h>
#include <cxxopts.hpp>
#include <memory>
#include <anim_localrotation.h>
#include <core_util.h>
#include <ecs_firstpersoncontrol.h>
#include <ecs_transformutil.h>
#include <ecsg_scenegraph.h>
#include <geo_primitivefactory.h>
#include <izz_materialsystem.h>
#include <izz_resourcemanager.h>
#include <wsp_workspace.h>

using namespace std;
using namespace lsw;
using namespace geo;
using lsw::core::Util;

std::shared_ptr<wsp::Workspace> parseProgramArguments(int argc, char* argv[]) {
  const std::string PROGRAM_NAME = "parallax";
  const std::string VERSION_STR = "1.0";
  cxxopts::Options _options(PROGRAM_NAME, "Parallax Example.\n");
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

int main(int argc, char** argv) {
#ifndef NDEBUG
  spdlog::set_level(spdlog::level::debug);
#endif  // NDEBUG

  auto workspace = parseProgramArguments(argc, argv);

  try {
    auto resourceManager = make_shared<ResourceManager>();
    auto sceneGraph = make_shared<ecsg::SceneGraph>();
    auto materialSystem = make_shared<MaterialSystem>(sceneGraph, resourceManager);
    materialSystem->loadMaterialsFromFile(workspace->materialsFile);
    resourceManager->setMaterialSystem(materialSystem);

    auto renderSystem = make_shared<glrs::RenderSystem>(sceneGraph, static_pointer_cast<glrs::IMaterialSystem>(materialSystem));
    auto viewer = std::make_shared<viewer::Viewer>(sceneGraph, renderSystem);

    // TODO: instead of resourceManager->createShared... change to: resourceManager->getMaterialSystem()->createSharedMaterial()
    //    sceneGraph->setDefaultMaterial(
    //        resourceManager->createSharedMaterial("DefaultMaterial"));

    // ==== SCENE SETUP ======================================================
    // TODO:
    auto boxL = sceneGraph->addGeometry(PrimitiveFactory::MakeBox("NormalBox"), *resourceManager->getMaterialSystem()->createMaterial("NormalMap"));
    boxL.translate(glm::vec3(-1.3F, 0.0F, 0.0F));
    //    boxL.add<anim::LocalRotation>({.radiansPerSecond = Util::ToRadians(-2.0F)});

    auto boxR = sceneGraph->addGeometry(PrimitiveFactory::MakeBox("ParallaxBox"), *resourceManager->getMaterialSystem()->createMaterial("ParallaxMap"));
    boxR.translate(glm::vec3(1.3F, 0.0F, 0.0F));
    //    boxR.add<anim::LocalRotation>({.radiansPerSecond = Util::ToRadians(2.0F)});

    auto sun = sceneGraph->makeDirectionalLight("Sun", glm::vec3(0.F, 1.0F, 1.0F));
    //    sun.add(anim::LocalRotation{Util::ToRadians(15.F)});

    // ==== CAMERA SETUP ====================================================
    auto camera = sceneGraph->makeCamera("DummyCamera", 8);
    auto& controls = camera.add<ecs::FirstPersonControl>();
    controls.onlyRotateOnMousePress = true;
    viewer->setActiveCamera(camera);

    // ==== UI SETUP ========================================================
    viewer->setWindowSize(1024, 768);
    viewer->setTitle("Normal mapping");
    viewer->initialize();
    viewer->run();
  } catch (std::runtime_error& e) {
    spdlog::error(e.what());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}