//
// Created by jlemein on 01-01-21.
//
#pragma once

#include "geo_boundingbox.h"

namespace izz {
namespace geo {

class ShapeUtil {
public:
  template <typename T> static BoundingBox computeBoundingBox(const T &shape) {

    if (shape.vertices.size() < 3) {
      return BoundingBox{};
    }

    // init bounding box with first vertex
    const auto &vertices = shape.vertices;
    BoundingBox bbox{vertices[0], vertices[0], vertices[1],
                     vertices[1], vertices[2], vertices[2]};

    // loop over the remaining vertices
    for (int i = 3; i < vertices.size(); i += 3) {
      bbox.xMin = vertices[i] < bbox.xMin ? vertices[i] : bbox.xMin;
      bbox.yMin = vertices[i + 1] < bbox.yMin ? vertices[i+1] : bbox.yMin;
      bbox.zMin = vertices[i + 2] < bbox.zMin ? vertices[i+2] : bbox.zMin;

      bbox.xMax = vertices[i] > bbox.xMax ? vertices[i] : bbox.xMax;
      bbox.yMax = vertices[i + 1] > bbox.yMax ? vertices[i+1] : bbox.yMax;
      bbox.zMax = vertices[i + 2] > bbox.zMax ? vertices[i+2] : bbox.zMax;
    }
    return bbox;
  }
};

} // end of package
} // end of enterprise
