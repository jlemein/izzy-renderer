//
// Created by jlemein on 06-11-20.
//

#ifndef INCLUDED_VIEWER_MESH_H_
#define INCLUDED_VIEWER_MESH_H_

#include <vector>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <iostream>

#include <shp_boundingbox.h>

namespace artifax {
namespace shp {

enum class PolygonMode {
  kTriangles,
  kQuads
};

struct Mesh {
  std::vector<float> vertices;
  std::vector<float> normals;
  std::vector<float> uvs;
  std::vector<uint32_t> indices;

  // describes the data
  PolygonMode polygonMode {PolygonMode::kTriangles};
};

} // end package
} // end enterprise

#endif // INCLUDED_VIEWER_MESH_H_
