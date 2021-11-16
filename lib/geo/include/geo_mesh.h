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
#include <memory>

#include <geo_boundingbox.h>
#include <geo_material.h>
#include <res_resource.h>

namespace lsw {
namespace geo {

enum class PolygonMode {
  kTriangles,
  kQuads
};


struct Mesh {
  std::string name;
  std::vector<float> vertices;
  std::vector<float> normals;
  std::vector<float> uvs;
  std::vector<float> tangents;
  std::vector<float> bitangents;

  std::vector<uint32_t> indices;

  std::shared_ptr<Material> material {nullptr};

  // describes the data
  PolygonMode polygonMode {PolygonMode::kTriangles};
};

} // end package
} // end enterprise

#endif // INCLUDED_VIEWER_MESH_H_
