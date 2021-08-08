//
// Created by jlemein on 11-03-21.
//
#include <memory>
#include <spdlog/spdlog.h>
#include <vwr_viewer.h>
#include <georm_materialsystem.h>
//#include <res_resourcemanager.h>
#include <ecs_firstpersoncontrol.h>
#include <ecsg_scenegraph.h>
#include <geo_primitivefactory.h>
#include <georm_materialsystem.h>
#include <georm_resourcemanager.h>
using namespace std;
using namespace lsw;
using namespace geo;

int main(int argc, char **argv) {
#ifndef NDEBUG
  spdlog::set_level(spdlog::level::debug);
#endif // NDEBUG

  try {
    auto resourceManager = make_shared<georm::ResourceManager>();

    auto sceneGraph = make_shared<ecsg::SceneGraph>();
    auto materialSystem =
        make_shared<georm::MaterialSystem>(sceneGraph, resourceManager);
    materialSystem->loadMaterialsFromFile("../assets/shaders/materials.json");
    resourceManager->setMaterialSystem(materialSystem);

    auto renderSystem = make_shared<ecs::RenderSystem>(sceneGraph, static_pointer_cast<ecs::IMaterialSystem>(materialSystem));

    sceneGraph->setDefaultMaterial(
        resourceManager->createSharedMaterial("DefaultMaterial"));
    
    auto lambert = resourceManager->createMaterial("NormalMapExample");

    sceneGraph->addGeometry(PrimitiveFactory::MakeBox(), lambert);

    auto viewer = std::make_shared<viewer::Viewer>(
        sceneGraph, renderSystem, resourceManager->getRawResourceManager());

    sceneGraph->makeDirectionalLight("Sun", glm::vec3(0.0F, 0.0F, 1.0F));

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