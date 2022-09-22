//
// Created by jlemein on 08-08-21.
//
#include <geo_mesh.h>
#include <geo_meshutil.h>
#include <geo_primitivefactory.h>
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <vector>

using namespace izz::geo;
using namespace testing;

class MeshUtilTest : public testing::Test {
 public:
};

TEST_F(MeshUtilTest, ShouldFailIfResourceNotExist) {
  Mesh mesh;

  mesh.vertices = {0.0F, 0.0F, 0.0F,  1.0F, 0.0F, 0.0F,  0.0F, 1.0F, 0.0F,   // triangle 1
                   1.0F, 0.0F, -1.0F, 1.0F, 1.0F, -1.0F, 1.0F, 1.0F, 0.0F};  // triangle 2
  mesh.uvs = {.0F, .0F, 1.F, 0.F, .0F, 1.F,                                  // triangle 1
              .0F, .0F, 1.F, .0F, .0F, 1.F};                                 // triangle 2
  mesh.indices = {0, 1, 2, 3, 4, 5};

  MeshUtil::GenerateTangentsAndBitangentsFromUvCoords(mesh);
  EXPECT_EQ(mesh.tangents.size(), mesh.vertices.size());

  // first triangle
  glm::vec3 T1(1.0F, 0.0F, 0.0F);
  EXPECT_EQ(mesh.tangents[0], T1.x);
  EXPECT_EQ(mesh.tangents[1], T1.y);
  EXPECT_EQ(mesh.tangents[2], T1.z);

  // second triangle
  glm::vec3 T2(0.0F, 0.0F, -1.0F);
  EXPECT_EQ(mesh.tangents[3], T1.x);
  EXPECT_EQ(mesh.tangents[4], T1.y);
  EXPECT_EQ(mesh.tangents[5], T1.z);
}

TEST_F(MeshUtilTest, ShouldFillVertices) {
  auto box = izz::geo::PrimitiveFactory::MakeBox("");
  for (int i = 0; i < box.vertices.size(); i++) {
    if (i % 3 == 0) std::cout << i / 3 << ": ";
    std::cout << box.vertices.at(i) << " ";
    if (i % 3 == 2) {
      std::cout << std::endl;
    }
  }
}

TEST_F(MeshUtilTest, ShouldFillVerticesFromIndex) {
  auto box = izz::geo::PrimitiveFactory::MakeBox("");
  for (int i = 0; i < box.indices.size(); i++) {
    auto vi = box.indices[i] * 3;
    if (i % 3 == 0) {
      std::cout << "triangle " << i / 3 << ": \t";
    }

    for (int n = 0; n < 3; ++n) {
      std::cout << box.vertices.at(vi + n) << " ";
    }
    std::cout << " (T: ";
    for (int n = 0; n < 3; ++n) {
      std::cout << box.tangents.at(vi + n) << " ";
    }
    std::cout << ") \t";

    std::cout << " (B: ";
    for (int n = 0; n < 3; ++n) {
      std::cout << box.bitangents.at(vi + n) << " ";
    }
    std::cout << ") \t";

    if (i % 3 == 2) {
      std::cout << std::endl;
    }
  }
}

TEST_F(MeshUtilTest, ShouldCreateBoxTangents) {
  auto box = izz::geo::PrimitiveFactory::MakeBox("");
  for (int t = 0; t < box.indices.size() / 3; t += 3) {
    auto index = box.indices[t];

    auto uvIndex = index * 2;
    std::cout << "UV1: [{" << box.uvs[uvIndex] << " " << box.uvs[uvIndex + 1] << "}, {" << box.uvs[uvIndex + 2] << " " << box.uvs[uvIndex + 3] << "}, {"
              << box.uvs[uvIndex + 4] << " " << box.uvs[uvIndex + 5] << "}]" << std::endl;

    for (int i = 0; i < 3; ++i) {
      auto Vx = box.vertices.at(index * 3 + i * 3);
      auto Vy = box.vertices.at(index * 3 + i * 3 + 1);
      auto Vz = box.vertices.at(index * 3 + i * 3 + 2);
      std::cout << "Vertex " << i << ": " << Vx << ", " << Vy << ", " << Vz << std::endl;
    }

    auto Tx = box.tangents[index * 3];
    auto Ty = box.tangents[index * 3 + 1];
    auto Tz = box.tangents[index * 3 + 2];
    auto Bx = box.tangents[index * 3];
    auto By = box.tangents[index * 3 + 1];
    auto Bz = box.tangents[index * 3 + 2];
    std::cout << "Tangent for t: " << t << ": " << Tx << ", " << Ty << ", " << Tz << std::endl;
    //        std::cout << "Bitangent for t: " << t << ": " << Bx << ", " << By << ", " << Bz << std::endl;
  }
  for (int i = 0; i < box.uvs.size(); i += 6) {
    std::cout << "UV " << i / 6 << ": " << box.uvs[i] << " " << box.uvs[i + 1] << " " << std::endl;
  }
  for (int i = 0; i < box.tangents.size(); i += 9) {
    std::cout << "Vertex " << i / 9 << ": " << box.vertices[i] << " " << box.vertices[i + 1] << " " << box.vertices[i + 2] << std::endl;

    std::cout << "Tangent " << i / 9 << ": " << box.tangents[i] << " " << box.tangents[i + 1] << " " << box.tangents[i + 2] << std::endl;
  }
}

TEST_F(MeshUtilTest, MeshVertex_Has_Hash) {
  Mesh::Vertex v1{1, 1, 2}, v2{2, 1, 2}, v3{2, 1, 2};

//  EXPECT_NE(v1, v2);
//  EXPECT_EQ(v2, v3);
}

TEST_F(MeshUtilTest, ShouldSmoothenNormalsSimple) {
  Mesh mesh;
  mesh.vertices = std::vector<float>{
      0.0, 1.0, 0.0,  //
      0.5, 0.5, 0.0,  //        |\    /|
      0.0, 0.0, 0.0,  //        | \  / |
      0.5, 0.5, 0.0,  //        |   *  |
      1.0, 1.0, 0.0,  //        |  / \ |
      1.0, 0.0, 0.0,  //        | /   \|
  };
  mesh.normals = std::vector<float>{-0.707106781, 0.707106781, 0.0,         -0.707106781, 0.707106781, 0.0,         -0.707106781, 0.707106781, 0.0,
                                     0.707106781, 0.707106781, 0.0,          0.707106781, 0.707106781, 0.0,          0.707106781, 0.707106781, 0.0};

  mesh.indices = std::vector<uint32_t>{0, 1, 2, 3, 4, 5};
  MeshUtil::GenerateSmoothNormals(mesh);

  EXPECT_EQ(mesh.vertices.size()/3, 5);
  EXPECT_EQ(mesh.indices.size(), 6);
  EXPECT_THAT(mesh.indices, ElementsAre(0, 1, 2, 1, 3, 4));


  EXPECT_NEAR(mesh.normals[0], -0.707106781, 0.00001);
  EXPECT_NEAR(mesh.normals[1], 0.707106781, 0.00001);
  EXPECT_NEAR(mesh.normals[2], 0.0, 0.00001);

  EXPECT_NEAR(mesh.normals[3], 0.0F, 0.00001);
  EXPECT_NEAR(mesh.normals[4], 1.0F, 0.00001);
  EXPECT_NEAR(mesh.normals[5], 0.0F, 0.00001);

  EXPECT_NEAR(mesh.normals[9], 0.707106781, 0.00001);
  EXPECT_NEAR(mesh.normals[10], 0.707106781, 0.00001);
  EXPECT_NEAR(mesh.normals[11], 0.0F, 0.00001);
}

TEST_F(MeshUtilTest, ShouldSmoothenNormals) {
  Mesh mesh;
  mesh.vertices = std::vector<float>{0.0, 1.0, 0.0, //      0 +      +
                                     0.5, 0.5, 1.0, //        |\    /|
                                     0.0, 0.0, 0.0, //        | \  / |
                                     0.0, 0.0, 0.0, //        |   *  |
                                     0.5, 0.5, 1.0, //        |  / \ |
                                     1.0, 0.0, 0.0, //        | /   \|
                                     1.0, 0.0, 0.0, //        +------+
                                     0.5, 0.5, 1.0,
                                     1.0, 1.0, 0.0};
  mesh.normals = std::vector<float>{-0.707106781, 0.707106781, 0.0,         -0.707106781, 0.707106781, 0.0,         -0.707106781, 0.707106781, 0.0,
                                    0.0,          0.707106781, 0.707106781, 0.0,          0.707106781, 0.707106781, 0.0,          0.707106781, 0.707106781,
                                    0.707106781,  0.707106781, 0.0,         0.707106781,  0.707106781, 0.0,         0.707106781,  0.707106781, 0.0};

  mesh.indices = std::vector<uint32_t>{0, 1, 2, 3, 4, 5, 6, 7, 8};
  MeshUtil::GenerateSmoothNormals(mesh);

  EXPECT_EQ(mesh.vertices.size()/3, 5);
  EXPECT_EQ(mesh.indices.size(), 9);
  EXPECT_THAT(mesh.indices, ElementsAre(0, 1, 2, 2, 1, 3, 3, 1, 4));


  // expect all normals are unit length
  for (auto i=0U; i<mesh.normals.size(); i+=3) {
    std::cout << "Normal " << i << ": " << mesh.normals[i] << ", " << mesh.normals[i+1] << " " << mesh.normals[i+2] << std::endl;
    glm::vec3* n = reinterpret_cast<glm::vec3*>(&mesh.normals[i]);
    EXPECT_NEAR(glm::length(*n), 1.0F, 0.0001);
  }
}