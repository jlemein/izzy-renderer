//
// Created by jlemein on 07-11-20.
//

#ifndef GLVIEWER_MESH_LOADER_H
#define GLVIEWER_MESH_LOADER_H

#include <string>

namespace artifax
{
namespace shp
{
class Mesh;

class MeshLoader
{
 public:
  static void loadSceneFile(const std::string &file, Mesh &mesh);

  static void createSimpleTriangles(Mesh &mesh);
};

} // end package
} // end enterprise

#endif // GLVIEWER_MESH_LOADER_H
