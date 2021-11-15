//
// Created by jlemein on 24-01-21.
//
#include <georm_sceneloader.h>
#include <geo_scene.h>
#include <gtest/gtest.h>
#include <stdlib.h>
#include <geo_mesh.h>
#include <geo_meshinstance.h>
using namespace lsw::georm;

class SceneLoaderTest : public testing::Test {
public:
};

TEST_F(SceneLoaderTest, ShouldFailIfResourceNotExist) {
  SceneLoader loader(nullptr);
  EXPECT_NO_THROW(loader.createResource("/home/jlemein/dev/artifax-renderer/lib/shapes/testdata/bunny.fbx"));
}

TEST_F(SceneLoaderTest, ShouldLoadTwoInstancedCubes) {
  // Loads a scene with 4 instances
  // - 3 instances are cubes named: Cube, Cube.001, Cube.002.
  // - All 3 cubes share the same mesh data. So there is 1 cube mesh.
  // - 2 cube instances share the same material: RedMaterial, the other has BlueMaterial.
  // - 1 instance is named: IcoSphere
  SceneLoader loader (nullptr);
  auto pScene = loader.createResource("/home/jlemein/dev/artifax-renderer/testassets/models/3objects.fbx");
  Scene* scene = reinterpret_cast<Scene*>(pScene.get());

  for (auto mesh : scene->meshes()) {
    std::cout << mesh->name << std::endl;
  }

  // TODO: it is impossible for assimp for instances to have their own materials.
  //  that's why meshes is 3 (versus 2 - cube and icosphere)
  EXPECT_EQ(scene->meshes().size(), 3U);
//  EXPECT_EQ(scene->meshInstances().size(), 4U);

  auto cube = scene->meshes()[0];
  auto cube1 = scene->meshes()[1];
  auto sphere = scene->meshes()[2];

  // mesh names
  EXPECT_EQ(cube->name, std::string{"Cube"});
  EXPECT_EQ(cube1->name, std::string{"Cube.001"});
  EXPECT_EQ(sphere->name, std::string{"Icosphere"});

  // materials assigned
  EXPECT_EQ((*cube->material)->name, std::string{"RedMaterial"});
  EXPECT_EQ((*cube->material)->diffuse.r, 1.0F);
  EXPECT_EQ((*cube->material)->diffuse.g, 0.0F);
  EXPECT_EQ((*cube->material)->diffuse.b, 0.0F);

  EXPECT_EQ((*cube1->material)->name, std::string{"BlueMaterial"});
  EXPECT_EQ((*cube1->material)->diffuse.r, 0.0F);
  EXPECT_EQ((*cube1->material)->diffuse.g, 0.0F);
  EXPECT_EQ((*cube1->material)->diffuse.b, 1.0F);

  EXPECT_EQ((*sphere->material)->name, std::string{"DefaultMaterial"});
}

TEST_F(SceneLoaderTest, PrintHierarchy) {
  // Loads a scene with 4 instances
  // - 3 instances are cubes named: Cube, Cube.001, Cube.002.
  // - All 3 cubes share the same mesh data. So there is 1 cube mesh.
  // - 2 cube instances share the same material: RedMaterial, the other has BlueMaterial.
  // - 1 instance is named: IcoSphere
  SceneLoader loader (nullptr);
  auto pScene = loader.createResource("/home/jlemein/dev/artifax-renderer/testassets/models/3objects.fbx");
  Scene* scene = reinterpret_cast<Scene*>(pScene.get());

  EXPECT_EQ(scene->rootNode()->name, "RootNode");
  EXPECT_EQ(scene->rootNode()->children.front()->transform[3].x, 0.0F);
  EXPECT_EQ(scene->rootNode()->children.front()->transform[3].y, 0.0F);
  EXPECT_EQ(scene->rootNode()->children.front()->transform[3].z, 0.0F);
  EXPECT_EQ(scene->rootNode()->children.front()->meshInstances[0]->transform[3].x, 0.0F);
  EXPECT_EQ(scene->rootNode()->children.front()->meshInstances[0]->transform[3].y, 0.0F);
  EXPECT_EQ(scene->rootNode()->children.front()->meshInstances[0]->transform[3].z, 0.0F);

}

//TEST_F(SceneLoaderTest, ShouldLoad2SceneFiles) {
//  // Loads a scene with 4 instances
//  // - 3 instances are cubes named: Cube, Cube.001, Cube.002.
//  // - All 3 cubes share the same mesh data. So there is 1 cube mesh.
//  // - 2 cube instances share the same material: RedMaterial, the other has BlueMaterial.
//  // - 1 instance is named: IcoSphere
//  SceneLoader loader (nullptr);
//
//  auto bunnyScene = static_pointer_cast<Scene>(loader.createResource("assets/models/bunny.fbx"));
//  auto bunnyMesh = bunnyScene->meshes()[0];
//  auto bunny = sceneGraph->makeMesh(*bunnyMesh);
//  bunny.add<FirstPersonControl>();
////  geo::MeshTransform::ScaleToUniformSize(bunny.get<geo::Mesh>());
//  TransformUtil::Translate(bunny.get<Transform>(), glm::vec3(4.0F, 1.0F, 1.0F));
//  bunny.add<Debug>(Debug{.shape = DebugShape::kEulerArrow});
//
//}

//TEST_F(SceneLoaderTest, ShouldLoadSimpleScene) {
//  SceneLoader loader;
//  loader.createResource("bunny.fbx");
//}