//
// Created by jlemein on 01-01-21.
//

#ifndef GLVIEWER_SHP_MESHINSTANCE_H
#define GLVIEWER_SHP_MESHINSTANCE_H

#include <memory>
#include <geo_transform.h>

namespace lsw {
namespace geo {

struct Mesh;
struct Material;

struct MeshInstance {
  std::string name {""};
  std::shared_ptr<Mesh> mesh {nullptr};
  std::shared_ptr<res::Resource<Material>> material {nullptr};
  glm::mat4 transform {glm::mat4(1.0F)};
};

} // end of package
} // end of enterprise

#endif // GLVIEWER_SHP_MESHINSTANCE_H
