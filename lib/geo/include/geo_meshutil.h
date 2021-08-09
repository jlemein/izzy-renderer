//
// Created by jlemein on 07-08-21.
//

#ifndef RENDERER_GEO_MESHUTIL_H
#define RENDERER_GEO_MESHUTIL_H

#include <glm/glm.hpp>

namespace lsw {
namespace geo {

class Mesh;

class MeshUtil {
public:


  static void GenerateTangents(geo::Mesh& m);

  static void ComputeTangent(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3,
                              const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3,
                              glm::vec3 &T, glm::vec3 &B);
};

} // namespace geo
} // namespace lsw

#endif // RENDERER_GEO_MESHUTIL_H
