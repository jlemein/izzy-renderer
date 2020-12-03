//
// Created by jlemein on 10-11-20.
//

#ifndef GLVIEWER_MESH_MESHTRANSFORM_H
#define GLVIEWER_MESH_MESHTRANSFORM_H

#include <glm/glm.hpp>

namespace artifax {

namespace shp
{
class Mesh;
class Curve;


/**!
 * @brief transforms the mesh data directly instead of applying it to the transformation
 * matrix.
 */
struct MeshTransform {
  static void scaleToUniformSize(Mesh &mesh, float size = 1.0F);
  static void scaleToUniformSize(Curve &curve, float size = 1.0F);
  static void centerMeshToWorldOrigin(Mesh &mesh);

  static void Translate(Mesh& mesh, const glm::vec3& translation);
  static void Rotate(Mesh& mesh, const glm::quat& quaternion);
};

}
}
#endif  // GLVIEWER_MESH_MESHTRANSFORM_H
