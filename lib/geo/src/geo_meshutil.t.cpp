//
// Created by jlemein on 08-08-21.
//
#include <gtest/gtest.h>
#include <geo_meshutil.h>
#include <geo_mesh.h>
#include <vector>
using namespace lsw::geo;

class MeshUtilTest : public testing::Test {
public:
};

TEST_F(MeshUtilTest, ShouldFailIfResourceNotExist) {
  Mesh mesh;

  mesh.vertices = {0.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F};
  mesh.uvs = {.0F, .0F, 1.F, 0.F, .0F, 1.F};
  mesh.indices = {0, 1, 2};

  MeshUtil::GenerateBinormalTangents(mesh);
  EXPECT_EQ(mesh.tangents.size(), mesh.vertices.size());

  glm::vec3 T1(1.0F, 0.0F, 0.0F);
  std::cout << "computed tangent: " << mesh.tangents[0] << " " << mesh.tangents[1] << " " << mesh.tangents[2] << std::endl;
  EXPECT_EQ(mesh.tangents[0], T1.x);
  EXPECT_EQ(mesh.tangents[1], T1.y);
  EXPECT_EQ(mesh.tangents[2], T1.z);
}
