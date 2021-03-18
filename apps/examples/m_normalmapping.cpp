//
// Created by jlemein on 11-03-21.
//
#include <spdlog/spdlog.h>
#include <geo_materialloader.h>
#include <memory>
#include <vwr_viewer.h>
#include <res_resourcemanager.h>
#include <ecsg_scenegraph.h>
#include <vwr_viewer.h>
using namespace std;
using namespace affx;

int main(int argc, char** argv) {
  auto resourceManager = make_shared<res::ResourceManager>();
  auto sceneGraph = make_shared<ecsg::SceneGraph>();

  auto sphere = PrimitiveFactory::CreateSphere();
  auto lambert = resourceManager->loadMaterial("Lambert");
  lambert->setDiffuseMap("assets/textures/diffuse_wall.png");
  lambert->setNormalMap("assets/textures/normal_wall.png");
  sceneGraph->addGeometry(sphere, lambert);

  auto viewer = make_shared<viewer::Viewer>(sceneGraph);
  viewer->setWindowSize(1024, 768);
  viewer->setTitle("Normal mapping");
//  viewer->initialize();
  viewer->run();

  return 0;
}