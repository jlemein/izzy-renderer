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

using namespace std;
using namespace lsw;
using namespace geo;
using lsw::core::Util;

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
    //    renderSystem->addSubsystem(make_shared<anim::AnimationSystem>());

    // TODO: instead of resourceManager->createShared... change to: resourceManager->getMaterialSystem()->createSharedMaterial()
    //    sceneGraph->setDefaultMaterial(
    //        resourceManager->createSharedMaterial("DefaultMaterial"));

    auto boxL = sceneGraph->addGeometry(PrimitiveFactory::MakeBox(), resourceManager->createMaterial("NormalMap1"));
    boxL.translate(glm::vec3(-0.7F, 0.0F, 0.0F));
    boxL.add<anim::LocalRotation>({.radiansPerSecond = Util::ToRadians(-2.0F), .axis = glm::vec3(0.0, 1.0, 0.0)});

    auto boxR = sceneGraph->addGeometry(PrimitiveFactory::MakeBox(), resourceManager->createMaterial("NormalMap2"));
    boxR.translate(glm::vec3(0.7F, 0.0F, 0.0F));
    boxR.add<anim::LocalRotation>({.radiansPerSecond = Util::ToRadians(2.0F), .axis = glm::vec3(0.0, 1.0, 0.0)});

    auto viewer = std::make_shared<viewer::Viewer>(sceneGraph, renderSystem, resourceManager->getRawResourceManager());

    auto sun = sceneGraph->makeDirectionalLight("Sun", glm::vec3(1.F, 0.F, 0.0F));
    //    sun.add(anim::LocalRotation{Util::ToRadians(10.0F)});

    auto dir = sun.get<ecs::Transform>().localTransform * glm::vec4(1.0, 1.0, 1.0, 0.0);
    std::cout << "LocalTransform: " << dir[0] << " " << dir[1] << " " << dir[2] << " " << dir[3] << std::endl;

    auto camera = sceneGraph->makeCamera("DummyCamera");
    auto& controls = camera.add<ecs::FirstPersonControl>();
    controls.onlyRotateOnMousePress = true;
    viewer->setActiveCamera(camera);

    viewer->setWindowSize(1024, 768);
    viewer->setTitle("Normal mapping");
    viewer->initialize();
    viewer->run();
  } catch (std::runtime_error& e) {
    spdlog::error(e.what());
    return -1;
  }

  return 0;
}