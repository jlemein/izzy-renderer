//
// Created by jlemein on 01-01-21.
//
#pragma once

#include <izz_boundingbox.h>

namespace izz {
namespace geo {

class ShapeUtil {
 public:
  template <typename T>
  static BoundingBox computeBoundingBox(const T& shape) {
    if (shape.vertices.size() < 3) {
      return BoundingBox{};
    }

    // init bounding box with first vertex
    const auto& vertices = shape.vertices;
//    BoundingBox bbox{glm::vec3{vertices[0], vertices[0], vertices[1]},
//                     glm::vec3{vertices[1], vertices[2], vertices[2]}};
    BoundingBox bbox{glm::vec3{vertices[0], vertices[1], vertices[2]},
          glm::vec3{vertices[0], vertices[1], vertices[2]}};

    // loop over the remaining vertices
    for (int i = 3; i < vertices.size(); i += 3) {
      bbox.min.x = vertices[i] < bbox.min.x ? vertices[i] : bbox.min.x;
      bbox.min.y = vertices[i + 1] < bbox.min.y ? vertices[i + 1] : bbox.min.y;
      bbox.min.z = vertices[i + 2] < bbox.min.z ? vertices[i + 2] : bbox.min.z;

      bbox.max.x = vertices[i] > bbox.max.x ? vertices[i] : bbox.max.x;
      bbox.max.y = vertices[i + 1] > bbox.max.y ? vertices[i + 1] : bbox.max.y;
      bbox.max.z = vertices[i + 2] > bbox.max.z ? vertices[i + 2] : bbox.max.z;
    }
    return bbox;
  }
};

}  // namespace geo
}  // namespace izz
