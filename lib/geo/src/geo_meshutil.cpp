//
// Created by jlemein on 07-08-21.
//
#include <geo_mesh.h>
#include <geo_meshutil.h>
using namespace lsw::geo;

namespace {
struct Uv {
  float u, v;
};
struct Vertex {
  float x, y, z;
};
} // namespace

void computeTangent(glm::vec3 e1, glm::vec3 e2, glm::vec2 delta_uv1,
                    glm::vec2 delta_uv2, glm::vec3 &T, glm::vec3 &B) {

  double det = delta_uv1.x * delta_uv2.y - delta_uv1.y * delta_uv2.x;
  double inv = 1.0 / det;

  T = glm::vec3{inv * (delta_uv2.y * e1.x - delta_uv1.x * e2.x),
                inv * (delta_uv2.y * e1.y - delta_uv1.x * e2.y),
                inv * (delta_uv2.y * e1.z - delta_uv1.x * e2.z)};

  B = glm::vec3{inv * (-delta_uv2.x * e1.x + delta_uv1.x * e2.x),
                inv * (-delta_uv2.x * e1.y + delta_uv1.x * e2.y),
                inv * (-delta_uv2.x * e1.z + delta_uv1.x * e2.z)};
}

void MeshUtil::GenerateBinormalTangents(geo::Mesh &m) {
  auto numTriangles = m.indices.size() / 3;

  for (auto i = 0U; i < numTriangles; ++i) {
    // take into account the offsets.
    // Looping through the triangles, each triangles has 3 indices, therefore
    // 3*i.
    int index0 = m.indices[3 * i];

    // each vertex has 3 coordinates, so the index must be multiplied by 3 to
    // end up at the starting X-coordinate of the referenced vertex.
    float *pVertex1 = m.vertices.data() + 3 * m.indices[index0];
    float *pVertex2 = m.vertices.data() + 3 * m.indices[index0 + 1];
    float *pVertex3 = m.vertices.data() + 3 * m.indices[index0 + 2];

    // for uvs we multiply by 2, since uvs consist of 2 coordinates.
    float *pUv1 = m.uvs.data() + 2 * m.indices[index0];
    float *pUv2 = m.uvs.data() + 2 * m.indices[index0 + 1];
    float *pUv3 = m.uvs.data() + 2 * m.indices[index0 + 2];

    glm::vec3 p1 = glm::vec3(pVertex1[0], pVertex1[1], pVertex1[2]);
    glm::vec3 p2 = glm::vec3(pVertex2[0], pVertex2[1], pVertex2[2]);
    glm::vec3 p3 = glm::vec3(pVertex3[0], pVertex3[1], pVertex3[2]);
    glm::vec2 uv1 = glm::vec2(pUv1[0], pUv1[1]);
    glm::vec2 uv2 = glm::vec2(pUv2[0], pUv2[1]);
    glm::vec2 uv3 = glm::vec2(pUv3[0], pUv3[1]);

    glm::vec3 e1 = p2 - p1;
    glm::vec3 e2 = p3 - p1;
    auto delta_uv1 = uv2 - uv1;
    auto delta_uv2 = uv3 - uv1;

    glm::vec3 T, B;
    computeTangent(e1, e2, delta_uv1, delta_uv2, T, B);

    for (int n = 0; n < 3; ++n) {
      m.tangents.emplace_back(T.x);
      m.tangents.emplace_back(T.y);
      m.tangents.emplace_back(T.z);
      m.bitangents.emplace_back(B.x);
      m.bitangents.emplace_back(B.y);
      m.bitangents.emplace_back(B.z);
    }
  }
}
