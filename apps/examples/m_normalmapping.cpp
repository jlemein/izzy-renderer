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

using namespace std;
using namespace lsw;
using namespace geo;
using lsw::core::Util;

void renderGui(float dt, float totalTime) {
  // set next window position. call before Begin(). use pivot=(0.5f,0.5f) to center on given point, etc.
  ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiCond_Once);
  ImGui::SetNextWindowSize(ImVec2(150, 300), ImGuiCond_Once);

  // render your GUI
  ImGui::Begin("Demo window");
  ImGui::Button("Hello!");
  ImGui::End();
}

int main(int argc, char** argv) {
#ifndef NDEBUG
  spdlog::set_level(spdlog::level::debug);
#endif  // NDEBUG

  try {
    auto resourceManager = make_shared<georm::ResourceManager>();
    auto sceneGraph = make_shared<ecsg::SceneGraph>();
    auto materialSystem = make_shared<georm::MaterialSystem>(sceneGraph, resourceManager);
    materialSystem->loadMaterialsFromFile("../assets/shaders/materials.json");
    resourceManager->setMaterialSystem(materialSystem);

    auto renderSystem = make_shared<ecs::RenderSystem>(sceneGraph, static_pointer_cast<ecs::IMaterialSystem>(materialSystem));
    auto viewer = std::make_shared<viewer::Viewer>(sceneGraph, renderSystem, resourceManager->getRawResourceManager());

    // TODO: instead of resourceManager->createShared... change to: resourceManager->getMaterialSystem()->createSharedMaterial()
    //    sceneGraph->setDefaultMaterial(
    //        resourceManager->createSharedMaterial("DefaultMaterial"));

    // ==== SCENE SETUP ======================================================
    auto boxL = sceneGraph->addGeometry(PrimitiveFactory::MakeBox(), resourceManager->createMaterial("NormalMap"));
    boxL.translate(glm::vec3(-1.3F, 0.0F, 0.0F));
    //    boxL.add<anim::LocalRotation>({.radiansPerSecond = Util::ToRadians(-2.0F)});

    auto boxR = sceneGraph->addGeometry(PrimitiveFactory::MakeBox(), resourceManager->createMaterial("ParallaxMap"));
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
    auto gui = std::make_shared<GuiSystem>(viewer, renderGui);
    viewer->registerExtension(gui);

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