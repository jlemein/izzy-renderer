//
// Created by jlemein on 24-01-21.
//
#include <geo_sceneloader.h>
#include <gtest/gtest.h>
#include <stdlib.h>
#include <geo_mesh.h>
using namespace affx::geo;

class SceneLoaderTest : public testing::Test {
};

TEST_F(SceneLoaderTest, ShouldFailIfResourceNotExist) {
  SceneLoader loader;
  EXPECT_NO_THROW(loader.loadResource("/home/jlemein/dev/artifax-renderer/lib/shapes/testdata/bunny.fbx"));
}

TEST_F(SceneLoaderTest, ShouldLoadTwoInstancedCubes) {
  // Loads a scene with 4 instances
  // - 3 instances are cubes named: Cube, Cube.001, Cube.002.
  // - All 3 cubes share the same mesh data. So there is 1 cube mesh.
  // - 2 cube instances share the same material: RedMaterial, the other has BlueMaterial.
  // - 1 instance is named: IcoSphere
  SceneLoader loader;
  //auto pScene = loader.loadResource("/home/jlemein/dev/artifax-renderer/lib/shapes/testdata/bunny.fbx"));
  auto pScene = loader.loadResource("/home/jlemein/dev/artifax-renderer/lib/shapes/testdata/three_instanced_cubes_one_sphere.fbx");
  Scene* scene = reinterpret_cast<Scene*>(pScene.get());

  for (auto mesh : scene->meshes()) {
    std::cout << mesh->name << std::endl;
  }

  // TODO: it is impossible for assimp for instances to have their own materials.
  //  that's why meshes is 3 (versus 2 - cube and icosphere)
  EXPECT_EQ(scene->meshes().size(), 3U);
  EXPECT_EQ(scene->meshInstances().size(), 4U);

  auto cube = scene->meshes()[0];
  auto cube1 = scene->meshes()[1];
  auto sphere = scene->meshes()[2];

  // mesh names
  EXPECT_EQ(cube->name, std::string{"Cube"});
  EXPECT_EQ(cube1->name, std::string{"Cube.001"});
  EXPECT_EQ(sphere->name, std::string{"Icosphere"});

  // materials assigned
  EXPECT_EQ(cube->material.name, std::string{"RedMaterial"});
  EXPECT_EQ(cube->material.diffuse.r, 1.0F);
  EXPECT_EQ(cube->material.diffuse.g, 0.0F);
  EXPECT_EQ(cube->material.diffuse.b, 0.0F);

  EXPECT_EQ(cube1->material.name, std::string{"BlueMaterial"});
  EXPECT_EQ(cube1->material.diffuse.r, 0.0F);
  EXPECT_EQ(cube1->material.diffuse.g, 0.0F);
  EXPECT_EQ(cube1->material.diffuse.b, 1.0F);

  EXPECT_EQ(sphere->material.name, std::string{"DefaultMaterial"});

}

//TEST_F(SceneLoaderTest, ShouldLoadSimpleScene) {
//  SceneLoader loader;
//  loader.loadResource("bunny.fbx");
//}