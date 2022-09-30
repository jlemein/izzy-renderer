//
// Created by jlemein on 11-03-21.
//

#include <anim_localrotation.h>
#include <core_util.h>
#include <ecs_firstpersoncontrol.h>
#include <ecs_light.h>
#include <ecs_transformutil.h>
#include <geo_primitivefactory.h>
#include <gui_lighteditor.h>
#include <gui_window.h>
#include <izz_fontsystem.h>
#include <izz_resourcemanager.h>
#include <spdlog/spdlog.h>
#include <wsp_workspace.h>
#include <cxxopts.hpp>
#include <memory>
#include "gui_iguiwindow.h"
#include "izz_entityfactory.h"
#include "izzgl_materialsystem.h"

using namespace std;
using namespace izz;
using namespace geo;
using izz::core::Util;
using namespace glm;
using namespace izz;

using izz::wsp::Workspace;

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
    workspace = wsp::WorkspaceManager::GetActiveWorkspace();
  } catch(std::runtime_error& e) {
    std::cerr << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }

  try {
    auto resourceManager = make_shared<ResourceManager>();
    auto fontSystem = make_shared<FontSystem>();
    auto sceneGraph = make_shared<izz::EntityFactory>();
    auto materialSystem = make_shared<MaterialSystem>(sceneGraph, resourceManager);

//    materialSystem->loadMaterialsFromFile(wsp::R("materials.json"));
    resourceManager->setMaterialSystem(materialSystem);

    auto renderSystem = make_shared<glrs::RenderSystem>(sceneGraph, static_pointer_cast<glrs::IMaterialSystem>(materialSystem));
    auto editor = make_shared<gui::LightEditor>(sceneGraph, fontSystem);
    auto guiSystem = make_shared<gui::GuiSystem>(fontSystem, editor);
    auto viewer = make_shared<gui::Window>(sceneGraph, renderSystem, guiSystem);

    // ==== SCENE SETUP ======================================================
    auto boxL = sceneGraph->addGeometry(PrimitiveFactory::MakeBox("Box"), *resourceManager->getMaterialSystem()->createMaterial("NormalMap"));
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