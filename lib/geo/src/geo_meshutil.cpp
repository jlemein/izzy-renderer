//
// Created by jlemein on 07-08-21.
//
#include <geo_mesh.h>
#include <geo_meshutil.h>
#include <glm/gtc/type_ptr.hpp>

using namespace lsw::geo;
using namespace glm;

namespace {
    /**
     * Computes the tangent and bitangent for a triangle p1, p2, p3.
     * E1 is the edge between p1 and p2, E2 is the edge between p
     * @param e1 Edge from vertex v0 to vertex v1.
     * @param e2
     * @param delta_uv1
     * @param delta_uv2
     * @param T
     * @param B
     */
    void computeTangent(glm::vec3 e1, glm::vec3 e2, glm::vec2 delta_uv1,
                        glm::vec2 delta_uv2, glm::vec3 &T, glm::vec3 &B) {

        double f = 1.0 / (delta_uv1.x * delta_uv2.y - delta_uv1.y * delta_uv2.x);

        T = glm::vec3{f * (delta_uv2.y * e1.x - delta_uv1.y * e2.x),
                      f * (delta_uv2.y * e1.y - delta_uv1.y * e2.y),
                      f * (delta_uv2.y * e1.z - delta_uv1.y * e2.z)};

        B = glm::vec3{f * (-delta_uv2.x * e1.x + delta_uv1.x * e2.x),
                      f * (-delta_uv2.x * e1.y + delta_uv1.x * e2.y),
                      f * (-delta_uv2.x * e1.z + delta_uv1.x * e2.z)};
    }
}

/**
 * Computes a tangent and bitangent vector for a triangle specified by the vertices v1, v2, v3 and the uv coordinates
 * uv1, uv2, uv3.
 * The order of v1, v2, v3 should be front-facing, usually counter-clockwise, similary for the corresponding uv coordinates.
 *
 * @param v1    [in]    Pointer to x attribute of vertex v1. Assumes v1 is a float[3] array, so that y and z follow x consecutively.
 * @param v2    [in]    Pointer to x attribute of vertex v2. Assumes v2 is a float[3] array, so that y and z follow x consecutively.
 * @param v3    [in]    Pointer to x attribute of vertex v2. Assumes v3 is a float[3] array, so that y and z follow x consecutively.
 * @param uv1   [in]    Pointer to u attribute of uv coordinate for v1. Assumes uv1 is a float[2] array, so that v follows consecutively.
 * @param uv2   [in]    Pointer to u attribute of uv coordinate for v2. Assumes uv2 is a float[2] array, so that v follows consecutively.
 * @param uv3   [in]    Pointer to u attribute of uv coordinate for v3. Assumes uv3 is a float[2] array, so that v follows consecutively.
 * @param T     [out]   Reference to tangent vector to be filled with tangent vector.
 * @param B     [out]   Reference to bitangent vector to be filled.
 */
void MeshUtil::ComputeTangent(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3,
                    const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3,
                    glm::vec3 &T, glm::vec3 &B) {

    computeTangent(p2 - p1, p3 - p1, uv2 - uv1, uv3 - uv1, T, B);
}

void MeshUtil::GenerateTangents(geo::Mesh &m) {
    auto numTriangles = m.indices.size() / 3;

    m.tangents.resize(m.vertices.size());
    m.bitangents.resize(m.vertices.size());

    for (auto i = 0U; i < numTriangles; ++i) {
        //
        int t0 = m.indices[3 * i];
        int t1 = m.indices[3 * i + 1];
        int t2 = m.indices[3 * i + 2];

        // each vertex has 3 coordinates, so the index must be multiplied by 3 to
        // end up at the starting X-coordinate of the referenced vertex.
        auto vertex1 = make_vec3(m.vertices.data() + 3 * t0);
        auto vertex2 = make_vec3(m.vertices.data() + 3 * t1);
        auto vertex3 = make_vec3(m.vertices.data() + 3 * t2);

        // for uvs we multiply by 2, since uvs consist of 2 coordinates.
        auto uv1 = make_vec2(m.uvs.data() + 2 * t0);
        auto uv2 = make_vec2(m.uvs.data() + 2 * t1);
        auto uv3 = make_vec2(m.uvs.data() + 2 * t2);

        glm::vec3 T, B;
        ComputeTangent(vertex1, vertex2, vertex3, uv1, uv2, uv3, T, B);

        float *pTangent1 = m.tangents.data() + 3 * t0;
        float *pTangent2 = m.tangents.data() + 3 * t1;
        float *pTangent3 = m.tangents.data() + 3 * t2;
        float *pBtangent1 = m.bitangents.data() + 3 * t0;
        float *pBtangent2 = m.bitangents.data() + 3 * t1;
        float *pBtangent3 = m.bitangents.data() + 3 * t2;

        pTangent1[0] = pTangent2[0] = pTangent3[0] = T.x;
        pTangent1[1] = pTangent2[1] = pTangent3[1] = T.y;
        pTangent1[2] = pTangent2[2] = pTangent3[2] = T.z;
        pBtangent1[0] = pBtangent2[0] = pBtangent3[0] = B.x;
        pBtangent1[1] = pBtangent2[1] = pBtangent3[1] = B.y;
        pBtangent1[2] = pBtangent2[2] = pBtangent3[2] = B.z;
    }
}
