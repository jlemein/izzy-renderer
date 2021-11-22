//
// Created by jlemein on 07-08-21.
//
#pragma once

#include <glm/glm.hpp>

namespace lsw {
namespace geo {

class Mesh;

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
  static void GenerateSmoothNormals(geo::Mesh& mesh);

  /**
   * Converts the normals, tangents and bitangents of the mesh to smooth normals.
   * The method changes the normals in place, meaning that the mesh normals, tangents and bitangents do not change size.
   * Calling this function assumes that the mesh already has normals, position data and indices assigned.
   * @param mesh The mesh containing already assigned flat surface normals, position data and index data.
   * @throws runtime_exception when mesh data is incomplete.
   */
  static void ConvertToSmoothNormals(geo::Mesh& mesh);

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
  static void GenerateFlatNormals(geo::Mesh& mesh);

  static void GenerateTangents(geo::Mesh& m);

  static void ComputeTangent(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3,
                              const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3,
                              glm::vec3 &T, glm::vec3 &B);
};

} // namespace geo
} // namespace lsw
