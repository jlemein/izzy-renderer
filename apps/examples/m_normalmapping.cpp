//
// Created by jlemein on 11-03-21.
//
#include <spdlog/spdlog.h>
#include <geo_materialloader.h>
#include <memory>
#include <vwr_viewer.h>
//#include <res_resourcemanager.h>
#include <georm_resourcemanager.h>
#include <ecsg_scenegraph.h>
#include <geo_primitivefactory.h>
using namespace std;
using namespace lsw;
using namespace geo;

int main(int argc, char** argv) {
#ifndef NDEBUG
  spdlog::set_level(spdlog::level::debug);
#endif // NDEBUG

  try {
    auto resourceManager =
        make_shared<georm::ResourceManager>("../assets/shaders/materials.json");

    auto sceneGraph = make_shared<ecsg::SceneGraph>();
    sceneGraph->setDefaultMaterial(
        resourceManager->createSharedMaterial("DefaultMaterial"));

    auto lambert = resourceManager->createMaterial("NormalMapExample");
//    lambert.setDiffuseMap("assets/textures/diffuse_wall.png");
//    lambert.setNormalMap("assets/textures/normal_wall.png");
//    lambert.setShaderLayout(UberMaterialData::PARAM_NAME);

    if (lambert.floatArrayValues.count("diffuse_color") > 0) {
      auto diffuse = lambert.floatArrayValues.at("diffuse_color");
      spdlog::info("Lambert has diffuse: {} {} {}", diffuse[0], diffuse[1], diffuse[2]);
    }

    sceneGraph->addGeometry(PrimitiveFactory::MakeBox(), lambert);

    auto viewer = std::make_shared<viewer::Viewer>(
        sceneGraph, resourceManager->getRawResourceManager());

    sceneGraph->makeDirectionalLight("Sun");

    viewer->setActiveCamera(sceneGraph->makeCamera("DummyCamera"));

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