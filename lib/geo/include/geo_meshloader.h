//
// Created by jlemein on 07-11-20.
//
#pragma once

#include <string>

namespace izz {
namespace geo
{
struct Mesh;

class MeshLoader
{
 public:
  static void loadSceneFile(const std::string &file, Mesh &mesh);

  static void createSimpleTriangles(Mesh &mesh);
};

} // end package
} // end enterprise
