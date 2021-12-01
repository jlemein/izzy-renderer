//
// Created by jlemein on 07-08-21.
//
#pragma once

#include <glm/glm.hpp>

namespace lsw {
namespace geo {

class Mesh;

/**
 * Contains utility methods to transform mesh data.
 * Methods are divided in three categories:
 */
class MeshUtil {
 public:
  /**
   * Generates smooth normals for a mesh. The mesh should have vertex data defined, otherwise no normals are generated.
   * The normals for the mesh can be empty when called. The method allocates enough space to exactly fit the number of
   * normals needed. If the normals are not empty it will clear and resize the vector of normals.
   *
   * @details Normals are calculated based on cross product calculation with vertex position data.
   * @param mesh The mesh for which to generate smooth normals.
   */
  //  static void GenerateSmoothNormals(geo::Mesh& mesh);

  /**
   * Generates flat normals for a mesh. The mesh should have vertex data defined, otherwise no normals are generated.
   * The normals for the mesh may be empty when called. The method allocates enough space to exactly fit the number of
   * normals needed. If the normals are not empty, it will clear and resize the vector of normals.
   * This method likely also changes the vertex data for the other vertex data attributes.
   *
   * @details Normals are calculated based on cross product calculation with vertex position data.
   * Flat normals also usually require duplication of vertex positions (and other attributes). This is the nature of flat
   * shading where a single vertex is part of multiple triangles.
   *
   * @param mesh The mesh for which to generate smooth normals.
   */
//  static void GenerateFlatNormals(geo::Mesh& mesh);

  /**
   * Converts the normals, tangents and bitangents of the mesh to smooth normals.
   * The method changes the normals in place, meaning that the mesh normals, tangents and bitangents do not change size.
   * Calling this function assumes that the mesh already has normals, position data and indices assigned.
   * If the mesh does not have tangents and/or bitangents defined, these attributes will be ignored and only normals
   * gets converted.
   * @param mesh The mesh containing already assigned flat surface normals, position data and index data.
   * @throws std::runtime_error when mesh data is incomplete.
   */
  static void GenerateSmoothNormals(geo::Mesh& mesh);

  /**
   * Generates bitangents for a mesh that contains vertex and tangent dat, using the cross product.
   * @param mesh Mesh with vertex and tangent data.
   */
  static void GenerateBitangentsByCrossProduct(geo::Mesh& mesh);

  /**
   * Given a mesh with normal data and uv coordinates defined, generate the tangent and bitangent data.
   * @throw std::runtime_error when mesh does not have data or uv coordinates defined.
   * @param [in, out] mesh Input mesh to compute the tangent and bitangent data for.
   */
  static void GenerateTangentsAndBitangentsFromUvCoords(geo::Mesh& mesh);

  /**
   * Computes the tangent and bitangent for vertex at position p1, defined by edges p1-p2 and p1-p3,
   * where uv1, uv2, uv3 correspond to the uv coordinates for p1, p2, p3 respectively.
   * @param [in] p1 Vertex position P1.
   * @param [in] p2 Vertex position P2.
   * @param [in] p3 Vertex position P3.
   * @param [in] uv1 Uv coordinate for vertex at position P1.
   * @param [in] uv2 Uv coordinate for vertex at position P2.
   * @param [in] uv3 Uv coordinate for vertex at position P3.
   * @param [out] T Computed Tangent.
   * @param [out] B Computed Bitangent.
   */
  static void ComputeTangentAndBitangent(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec2& uv1, const glm::vec2& uv2,
                                         const glm::vec2& uv3, glm::vec3& T, glm::vec3& B);

  /**
   * Scales the uv coordinates by u over the u-axis and v over the v-axis.
   * The uv coordinates are wrapped in the range [0, 1].
   *
   * @param mesh Mesh to scale the uv coordinates for.
   * @param u Scale factor for u axis.
   * @param v Scale factor for v axis.
   */
  static void ScaleUvCoords(geo::Mesh& mesh, float u, float v);
};

}  // namespace geo
}  // namespace lsw
