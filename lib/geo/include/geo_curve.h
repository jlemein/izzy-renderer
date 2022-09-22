//
// Created by jlemein on 09-11-20.
//
#pragma once

#include <geo_boundingbox.h>
#include <vector>

namespace izz {
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
