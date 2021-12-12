//
// Created by jlemein on 11-03-21.
//
#include "../lib/core/include/georm_materialsystem.h"
#include <memory>
#include <spdlog/spdlog.h>
#include <gui_window.h>
#include "../lib/core/include/anim_localrotation.h"
#include <core_util.h>
#include <ecs_firstpersoncontrol.h>
#include <ecs_transformutil.h>
#include <ecsg_scenegraph.h>
#include <geo_primitivefactory.h>
#include <georm_materialsystem.h>
#include <georm_resourcemanager.h>

using namespace std;
using namespace lsw;
using namespace izz;
using namespace geo;
using lsw::core::Util;

int main(int argc, char **argv) {
#ifndef NDEBUG
  spdlog::set_level(spdlog::level::debug);
#endif // NDEBUG

//    std::cout << "Usage: program --materials=materials.json --scene=hello.fbx"

  try {
    auto resourceManager = make_shared<ResourceManager>();
    auto sceneGraph = make_shared<ecsg::SceneGraph>();
    auto materialSystem =
        make_shared<MaterialSystem>(sceneGraph, resourceManager);
    materialSystem->loadMaterialsFromFile("../assets/shaders/materials.json");
    resourceManager->setMaterialSystem(materialSystem);

    auto renderSystem = make_shared<ecs::RenderSystem>(
        sceneGraph, static_pointer_cast<ecs::IMaterialSystem>(materialSystem));
    //    renderSystem->addSubsystem(make_shared<anim::AnimationSystem>());

    sceneGraph->setDefaultMaterial(
        resourceManager->createSharedMaterial("DefaultMaterial"));

    auto lambert = resourceManager->createMaterial("NormalMapExample");

    auto box = sceneGraph->addGeometry(PrimitiveFactory::MakeBox(), lambert);
        box.add<anim::LocalRotation>(
            {.radiansPerSecond = Util::ToRadians(2.0F),
             .axis = glm::vec3(0.0, 1.0, 0.0)});

    auto viewer = std::make_shared<gui::Window>(
        sceneGraph, renderSystem, resourceManager->getRawResourceManager());

    auto sun =
        sceneGraph->makeDirectionalLight("Sun", glm::vec3(1.F, 2.F, 0.0F));
    sun.add(anim::LocalRotation{Util::ToRadians(10.0F)});

    auto camera = sceneGraph->makeCamera("DummyCamera");
    camera.add<ecs::FirstPersonControl>();
    viewer->setActiveCamera(camera);

    viewer->setWindowSize(1024, 768);
    viewer->setTitle("Normal mapping");
    viewer->initialize();
    viewer->run();
  } catch (std::runtime_error &e) {
    spdlog::error(e.what());
    return -1;
  }

  return 0;
}