#include <iostream>
#include <core_util.h>
#include <ecs_firstpersoncontrol.h>
#include <ecsg_scenegraph.h>
#include <geo_primitivefactory.h>
#include <georm_materialsystem.h>
#include <georm_resourcemanager.h>
#include <georm_materialsystem.h>
#include <ecs_light.h>
#include <vwr_viewer.h>
#include <ecs_wireframe.h>

using namespace std;
using namespace lsw;
using namespace geo;
using lsw::core::Util;
using namespace glm;

int main(int argc, char* argv[]) {
  cout << "Hello Icosphere" << endl;

  try {
    auto resourceManager = make_shared<georm::ResourceManager>();
    auto sceneGraph = make_shared<ecsg::SceneGraph>();
    auto materialSystem = make_shared<georm::MaterialSystem>(sceneGraph, resourceManager);
    materialSystem->loadMaterialsFromFile("materials.json");
    resourceManager->setMaterialSystem(materialSystem);

    auto renderSystem = make_shared<ecs::RenderSystem>(sceneGraph, static_pointer_cast<ecs::IMaterialSystem>(materialSystem));

//    auto box = sceneGraph->addGeometry(PrimitiveFactory::MakeBox(), resourceManager->createMaterial("Wireframe"));
    auto box = sceneGraph->addGeometry(PrimitiveFactory::MakeUVSphere(), resourceManager->createMaterial("Wireframe"));
    box.add<ecs::Wireframe>();

    auto viewer =
        make_shared<viewer::Viewer>(sceneGraph, renderSystem, resourceManager->getRawResourceManager() /*, guiSystem*/);

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