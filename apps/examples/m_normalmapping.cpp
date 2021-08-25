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

using lsw::wsp::Workspace;

namespace {
#ifndef NDEBUG
const char* DEBUG_MODE = "true";
#else
const char* DEBUG_MODE = "false";
#endif  // NDEBUG
}  // namespace

#include <fstream>

int main(int argc, char* argv[]) {
#ifndef NDEBUG
  spdlog::set_level(spdlog::level::debug);
#endif

  std::shared_ptr<Workspace> workspace {nullptr};

  try {
    workspace = wsp::WorkspaceFactory::CreateDefaultWorkspace();
    wsp::WorkspaceManager::SetActiveWorkspace(workspace);
  } catch(std::runtime_error& e) {
    std::cerr << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }

  try {
    auto resourceManager = make_shared<georm::ResourceManager>();
    auto fontSystem = make_shared<georm::FontSystem>();
    auto sceneGraph = make_shared<ecsg::SceneGraph>();
    auto materialSystem = make_shared<georm::MaterialSystem>(sceneGraph, resourceManager);

    materialSystem->loadMaterialsFromFile(wsp::R("materials.json"));
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