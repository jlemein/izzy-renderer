//
// Created by jlemein on 13-01-21.
//

#ifndef GLVIEWER_ECSW_MESHINSTANCE_H
#define GLVIEWER_ECSW_MESHINSTANCE_H

#include <memory>

namespace lsw {
namespace geo {
struct Mesh;
}

namespace ecsw {

class Material;

class MeshInstance {
  std::shared_ptr<geo::Mesh> m_mesh;
  std::shared_ptr<Material> m_material;
  std::string m_name;
};

} // end of package
} // end of enterprise

#endif // GLVIEWER_ECSW_MESHINSTANCE_H
