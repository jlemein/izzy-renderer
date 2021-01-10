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

  // Not used for now
//  std::vector<uint32_t> indices;

  float width;
};

} // end package
} // end enterprise

#endif // GLVIEWER_VIEWER_CURVE_H
