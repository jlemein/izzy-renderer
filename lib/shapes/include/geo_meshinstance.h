//
// Created by jlemein on 01-01-21.
//

#ifndef GLVIEWER_SHP_MESHINSTANCE_H
#define GLVIEWER_SHP_MESHINSTANCE_H

#include <memory>
#include <geo_transform.h>

namespace affx {
namespace geo {

struct Mesh;
struct Material;

struct MeshInstance {
  std::shared_ptr<Mesh> mMesh;
  std::shared_ptr<Material> mMaterial;
};

} // end of package
} // end of enterprise

#endif // GLVIEWER_SHP_MESHINSTANCE_H
