//
// Created by jlemein on 09-11-20.
//

#ifndef AFFX_GEO_CURVE_H
#define AFFX_GEO_CURVE_H

#include <geo_boundingbox.h>
#include <vector>

namespace lsw
{
namespace geo
{
struct Curve {
  std::vector<float> vertices;

  // Not used for now
//  std::vector<uint32_t> indices;

  float width;
};

} // end package
} // end enterprise

#endif // AFFX_GEO_CURVE_H
