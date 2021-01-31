//
// Created by jlemein on 01-12-20.
//

#ifndef AFFX_GEO_BOUNDINGBOX_H
#define AFFX_GEO_BOUNDINGBOX_H

#include <ostream>
#include <glm/glm.hpp>

namespace affx {
namespace geo {
struct BoundingBox {
  float xMin{.0F}, yMin{.0F}, zMin{.0F};
  float xMax{.0F}, yMax{.0F}, zMax{.0F};
};

// std::ostream& operator<<(std::ostream& os, const BoundingBox& bb) {
//  return os;
//}
struct BoundingBoxUtil {
public:
  static glm::vec3 getSize(const BoundingBox &bb) {
    return glm::vec3{bb.xMax - bb.xMin, bb.yMax - bb.yMin, bb.zMax - bb.zMin};
  }

  static glm::vec3 getCenter(const BoundingBox &bb) {
    return glm::vec3{bb.xMin, bb.yMin, bb.zMin} + 0.5F * getSize(bb);
  }
};
} // end of package
} // end of enterprise

#endif // AFFX_GEO_BOUNDINGBOX_H
