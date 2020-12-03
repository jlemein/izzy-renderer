//
// Created by jlemein on 09-11-20.
//

#ifndef GLVIEWER_VIEWER_CURVE_H
#define GLVIEWER_VIEWER_CURVE_H

#include <shp_boundingbox.h>
#include <vector>

namespace artifax
{
namespace shp
{
struct Curve {
  std::vector<float> vertices;
  float width;

  BoundingBox
  getBoundingBox()
  {
    BoundingBox bbox;
    for (int i = 0; i < vertices.size(); i += 3) {
      bbox.xMin = vertices[i] < bbox.xMin ? vertices[i] : bbox.xMin;
      bbox.yMin = vertices[i + 1] < bbox.yMin ? vertices[i] : bbox.yMin;
      bbox.zMin = vertices[i + 2] < bbox.zMin ? vertices[i] : bbox.zMin;

      bbox.xMax = vertices[i] > bbox.xMax ? vertices[i] : bbox.xMax;
      bbox.yMax = vertices[i + 1] > bbox.yMax ? vertices[i] : bbox.yMax;
      bbox.zMax = vertices[i + 2] > bbox.zMax ? vertices[i] : bbox.zMax;
    }
    return bbox;
  }
};

} // end package
} // end enterprise

#endif // GLVIEWER_VIEWER_CURVE_H
