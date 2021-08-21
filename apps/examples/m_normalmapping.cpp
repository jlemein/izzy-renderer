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
#include <wsp_workspace.h>
#include <georm_fontsystem.h>
#include <ecs_light.h>
#include <cxxopts.hpp>

using namespace std;
using namespace lsw;
using namespace geo;
using lsw::core::Util;
using namespace glm;

struct File {
  std::string path;
  void* data;

  void openReadOnly();
};

/// @brief The R class is a resource locator class.
/// The main use of this class is to have a concise way to resolve resources from the
/// workspace directory. The R class makes use of a singleton that gets initialized in the
/// beginning of the simulation.
// R("textures/skydome.jpg")

using lsw::wsp::Workspace;

namespace {
#ifndef NDEBUG
const char* DEBUG_MODE = "true";
#else
const char* DEBUG_MODE = "false";
#endif  // NDEBUG
}  // namespace

Workspace parseProgramArguments(int argc, char* argv[]) {
  cxxopts::Options _options("normalmap", "Demo of normal mapping");
  _options.add_options()
      ("d,debug", "Enable debugging", cxxopts::value<bool>()->default_value(DEBUG_MODE))
      ("s,scene", "A scene file for visualization (*.fbx, *.obj)", cxxopts::value<std::string>())
      ("m,materials", "Reference to a materials json file", cxxopts::value<std::string>()->default_value(""))
      ("w,workspace", "Workspace directory", cxxopts::value<std::string>())
      ("h,help","Print usage");
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

  auto& workspace = wsp::WorkspaceManager::GetWorkspace();
  workspace.sceneFile = result["scene"].as<std::string>();
  workspace.materialsFile = result["materials"].as<std::string>();

  const char* weraHomeEnv = getenv("WERA3D_HOME");
  if (weraHomeEnv) {
    workspace.installDir = weraHomeEnv;
    workspace.path = argv[0];
  } else {
    workspace.isStrictMode = true;  // no default materials to read from, which implies strict mode.
  }

  workspace.debugMode = result["debug"].as<bool>();

  cout << "Scene file: " << workspace.sceneFile << endl;
  cout << "Materials file: " << (workspace.materialsFile.empty() ? "<not specified>" : workspace.materialsFile) << endl;
  cout << "Strict mode: " << (workspace.isStrictMode ? "enabled" : "disabled") << endl;

  cout << "Testing R: " << wsp::R("myTexture.jpg") << std::endl;

  return workspace;
}

int main(int argc, char* argv[]) {
  auto workspace = parseProgramArguments(argc, argv);
  if (workspace.debugMode) {
    spdlog::set_level(spdlog::level::debug);
  }

  try {
    auto resourceManager = make_shared<georm::ResourceManager>();
    auto fontSystem = make_shared<georm::FontSystem>(workspace.path.c_str());
    auto sceneGraph = make_shared<ecsg::SceneGraph>();
    auto materialSystem = make_shared<georm::MaterialSystem>(sceneGraph, resourceManager);
    materialSystem->loadMaterialsFromFile(workspace.path / "materials.json");
    resourceManager->setMaterialSystem(materialSystem);

    auto renderSystem = make_shared<ecs::RenderSystem>(sceneGraph, static_pointer_cast<ecs::IMaterialSystem>(materialSystem));
    auto editor = make_shared<gui::GuiLightEditor>(sceneGraph, fontSystem);
    auto guiSystem = make_shared<GuiSystem>(editor);
    auto viewer = make_shared<viewer::Viewer>(sceneGraph, renderSystem, resourceManager->getRawResourceManager(), guiSystem);

    // TODO: instead of resourceManager->createShared... change to: resourceManager->getMaterialSystem()->createSharedMaterial()
    //    sceneGraph->setDefaultMaterial(
    //        resourceManager->createSharedMaterial("DefaultMaterial"));

    // ==== SCENE SETUP ======================================================
    auto boxL = sceneGraph->addGeometry(PrimitiveFactory::MakeBox(), resourceManager->createMaterial("NormalMap"));
    boxL.translate(glm::vec3(0.0F, 0.0F, 0.0F));
    //    boxL.add<anim::LocalRotation>({.radiansPerSecond = Util::ToRadians(-2.0F)});

    //    auto boxR = sceneGraph->addGeometry(PrimitiveFactory::MakeBox(), resourceManager->createMaterial("ParallaxMap"));
    //    boxR.translate(glm::vec3(1.3F, 0.0F, 0.0F));
    //    boxR.add<anim::LocalRotation>({.radiansPerSecond = Util::ToRadians(2.0F)});

    // ==== LIGHTS SETUP ====================================================
    auto sun = sceneGraph->makeDirectionalLight("Sun", glm::vec3(0.F, 1.0F, 1.0F));
    //    sun.add(anim::LocalRotation{Util::ToRadians(15.F)});
    sceneGraph->makeAmbientLight("Ambient");
    //    sceneGraph->makePointLight("PointLight1", glm::vec3{0.0F, 2.0F, 0.0F}, ecs::PointLight{.color = {1.F, 0.F, 0.F}});
    sceneGraph->makePointLight("PointLight2", glm::vec3{0.0F, .5F, 0.0F}, ecs::PointLight{.color = {0.F, 1.F, 0.F}});
    //    sceneGraph->makePointLight("Bulb1", glm::vec3{1.F, 1.0F, 1.0F}, ecs::PointLight{.color = {1.F, .5F, .0F}});
    //    sceneGraph->makePointLight("Bulb2", glm::vec3{0.F, 1.0F, 1.0F}, ecs::PointLight{.color = {0.F, .5F, .1F}});

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