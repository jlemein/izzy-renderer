//
// Created by jlemein on 01-12-20.
//
#include <geo_curve.h>
#include <geo_mesh.h>
#include <geo_primitivefactory.h>
#include <geo_meshutil.h>
using namespace lsw;
using namespace lsw::geo;

Mesh PrimitiveFactory::MakePlane(float width, float height) { return Mesh(); }

Mesh PrimitiveFactory::MakeBox(float width, float height, float depth) {
  float vertices[] = {
      -.5F, -.5F, .5F,  // v0
      .5F,  -.5F, .5F,  // v1
      .5F,  -.5F, -.5F, // v2
      -.5F, -.5F, -.5F, // v3
      -.5F, .5F,  .5F,  // v4
      .5F,  .5F,  .5F,  // v5
      .5F,  .5F,  -.5F, // v6
      -.5F, .5F,  -.5F, // v7

      -.5F, -.5F, .5F,  // v8
      .5F,  -.5F, .5F,  // v9
      .5F,  -.5F, -.5F, // v10
      -.5F, -.5F, -.5F, // v11
      -.5F, .5F,  .5F,  // v12
      .5F,  .5F,  .5F,  // v13
      .5F,  .5F,  -.5F, // v14
      -.5F, .5F,  -.5F, // v15

      -.5F, -.5F, .5F,  // v16
      .5F,  -.5F, .5F,  // v17
      .5F,  -.5F, -.5F, // v18
      -.5F, -.5F, -.5F, // v19
      -.5F, .5F,  .5F,  // v20
      .5F,  .5F,  .5F,  // v21
      .5F,  .5F,  -.5F, // v22
      -.5F, .5F,  -.5F  // v23
  };

  float normals[] = {
      .0F,  .0F,  1.F,  .0F,  .0F,  1.F,  // v0 v1
      .0F,  .0F,  -1.F, .0F,  .0F,  -1.F, // v2 v3
      .0F,  .0F,  1.F,  .0F,  .0F,  1.F,  // v4 v5
      .0F,  .0F,  -1.F, .0F,  .0F,  -1.F, // v6 v7

      -1.F, .0F,  .0F,  1.F,  .0F,  .0F, // v8 v9
      1.F,  .0F,  .0F,  -1.F, .0F,  .0F, // v10 v11
      -1.F, .0F,  .0F,  1.F,  .0F,  .0F, // v12 v13
      1.F,  .0F,  .0F,  -1.F, .0F,  .0F, // v14 v15

      .0F,  -1.F, .0F,  .0F,  -1.F, .0F, // v16 v17
      .0F,  -1.F, .0F,  .0F,  -1.F, .0F, // v18 v19
      .0F,  1.F,  .0F,  .0F,  1.F,  .0F, // v20 v21
      .0F,  1.F,  .0F,  .0F,  1.F,  .0F  // v22 v23
  };

  float uvs[] = {
      0.0F, 0.0F, 1.0,  0.0F, 0.0,  0.0F, 1.0F, 0.0F, // 0 1 2 3
      0.0F, 1.0F, 1.0,  1.0F, 0.0,  1.0F, 1.0F, 1.0F, // 4 5 6 7
      1.0F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F, 0.0F, // 8 9 10 11
      1.0F, 1.0F, 0.0F, 1.0F, 1.0F, 1.0F, 0.0F, 1.0F, // 12 13 14 15
      0.0F, 1.0F, 1.0F, 1.0F, 1.0F, 0.0F, 0.0F, 0.0F, // 16 17 18 19
      0.0F, 0.0F, 1.0F, 0.0F, 1.0F, 1.0F, 0.0F, 1.0F  // 20 21 22 23
  };

  /**
   *          v7/15/23 ------------ v6/14/22
   *        /  |             / |
   *      /    |           /   |
   *     v4/12/20 ------------ v5/13/21    |
   *     |     |          |    |
   *     |    v3/11/19 ---------|-- v2/10/18
   *     |   /            |   /
   *     | /              | /
   *     v0/8/16 ------------ v1/9/17
   *
   *  v x/y/z --> x is front/back, y is left/right, z is bottom/top
   */

  uint32_t indices[] = {0,  1,  5,  0,  5,  4,   // Front
                        9,  10, 14, 9,  14, 13,  // Right
                        2,  3,  7,  2,  7,  6,   // Back
                        11, 8,  12, 11, 12, 15,  // Left
                        16, 19, 17, 17, 19, 18,  // Bottom
                        20, 21, 22, 20, 22, 23}; // Top
  Mesh mesh;
  mesh.vertices =
      std::vector<float>{vertices, vertices + sizeof(vertices) / sizeof(float)};
  mesh.indices = std::vector<uint32_t>{indices, indices + sizeof(indices) /
                                                              sizeof(uint32_t)};
  mesh.uvs = std::vector<float>{uvs, uvs + sizeof(uvs) / sizeof(float)};
  mesh.normals =
      std::vector<float>{normals, normals + sizeof(normals) / sizeof(float)};

  // scale the box to requested size
  for (unsigned int i = 0U; i < mesh.vertices.size(); i += 3) {
    mesh.vertices[i] *= width;
    mesh.vertices[i + 1] *= height;
    mesh.vertices[i + 2] *= depth;
  }

  MeshUtil::GenerateTangents(mesh);

  return mesh;
}

Mesh PrimitiveFactory::MakeCylinder(float radius, float height, int numSides) {
  Mesh mesh;

  // cap + bottom + sides
  auto numVertices = numSides * 2 + 2;
  auto numTriangles = numSides * 4;
  mesh.vertices.reserve(numVertices * 3);
  mesh.indices.reserve(numTriangles * 3);

  float cap[6] = {0.0F, -0.5F * height, 0.0F,  // bottom cap
                  0.0F, 0.5F * height,  0.0F}; // top cap
  std::copy(cap, cap + 6, std::back_inserter(mesh.vertices));

  for (int i = 0; i < 2 * numSides; ++i) {
    float t = 2.0F * M_PI * static_cast<float>(i % numSides) / numSides;
    float y = i < numSides ? -.5F * height : .5 * height;
    float vertex[3] = {radius * std::sin(t), y, radius * std::cos(t)};
    std::copy(vertex, vertex + 3, std::back_inserter(mesh.vertices));
  }

  // faces for the top cap
  for (int i = 0; i < numSides; ++i) {
    uint32_t baseVertex = 2U;
    uint32_t t0 = 0U;
    auto t1 = baseVertex + (i + 1U) % numSides;
    auto t2 = baseVertex + i;

    uint32_t topFace[3] = {t0, t1, t2};
    std::copy(topFace, topFace + 3, std::back_inserter(mesh.indices));
  }

  // faces for the bottom cap
  for (int i = 0; i < numSides; ++i) {
    uint32_t baseVertex = 2U;
    uint32_t t0 = 1U;
    auto t1 = baseVertex + numSides + (i + 1U) % numSides;
    auto t2 = baseVertex + numSides + i;

    uint32_t bottomFace[3] = {t0, t1, t2};
    std::copy(bottomFace, bottomFace + 3, std::back_inserter(mesh.indices));
  }

  // add triangles for the sides of the cylinder
  for (int i = 0; i < numSides; ++i) {
    uint32_t baseOffset = 2;
    auto t0 = baseOffset + i;
    auto t1 = baseOffset + (i + 1) % numSides;
    auto t2 = baseOffset + numSides + i;
    auto t3 = baseOffset + numSides + (i + 1) % numSides;

    uint32_t triangleIndices[6] = {t0, t1, t2, t2, t1, t3};
    std::copy(triangleIndices, triangleIndices + 6,
              std::back_inserter(mesh.indices));
  }

  return mesh;
}

Mesh PrimitiveFactory::MakePyramid(float baseWidth, float height) {
  float vertices[] = {
      -.5F, -.5F, -.5F, // v0
      .5F,  -.5F, -.5F, // v1
      .5F,  -.5F, .5F,  // v2
      -.5F, -.5F, .5F,  // v3
      .0F,  .5F,  .0F   // v4 - top of pyramid
  };

  uint32_t indices[] = {
      0, 2, 1, 0, 3, 1, // Base
      0, 4, 3,          // Left
      0, 1, 4,          // Front
      1, 2, 4,          // Right
      2, 3, 4,          // Back
  };
  Mesh mesh;
  mesh.vertices =
      std::vector<float>{vertices, vertices + sizeof(vertices) / sizeof(float)};
  mesh.indices = std::vector<uint32_t>{indices, indices + sizeof(indices) /
                                                              sizeof(uint32_t)};

  // scale the box to requested size
  for (unsigned int i = 0U; i < mesh.vertices.size(); i += 3) {
    mesh.vertices[i] *= baseWidth;
    mesh.vertices[i + 1] *= height;
    mesh.vertices[i + 2] *= baseWidth;
  }

  return mesh;
}
Mesh PrimitiveFactory::MakeSphere(float radius) { return Mesh(); }

Mesh PrimitiveFactory::MakeDonut(float innerRadius, float outerRadius) {
  return Mesh();
}
Mesh PrimitiveFactory::MakeTeapot(float size) { return Mesh(); }
