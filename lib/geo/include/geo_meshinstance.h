//
// Created by jlemein on 01-01-21.
//
#pragma once

#include <memory>
#include <geo_transform.h>

namespace lsw {
namespace geo {

struct Mesh;
struct Material;

struct MeshInstance {
  std::string name {""};
  std::shared_ptr<Mesh> mesh {nullptr};
//  std::shared_ptr<Material> material {nullptr};
  int materialId {-1};
  glm::mat4 transform {glm::mat4(1.0F)};
};

} // end of package
} // end of enterprise
