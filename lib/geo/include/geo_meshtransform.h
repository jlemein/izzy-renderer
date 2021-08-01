//
// Created by jlemein on 10-11-20.
//

#ifndef GLVIEWER_MESH_MESHTRANSFORM_H
#define GLVIEWER_MESH_MESHTRANSFORM_H

#include <glm/glm.hpp>

namespace lsw {

namespace geo
{
struct Mesh;
struct Curve;


/**!
 * @brief transforms the mesh data directly instead of applying it to the transformation
 * matrix. This can be more efficient, since you are not manipuling the scene
 * hierarchy
 */
struct MeshTransform {
  static void ScaleToUniformSize(Mesh& mesh, float size = 1.0F);
  static void ScaleToUniformSize(Curve &curve, float size = 1.0F);
  static void CenterMeshToWorldOrigin(Mesh &mesh);

  static void Translate(Mesh& mesh, const glm::vec3& translation);
  static void Rotate(Mesh& mesh, const glm::quat& quaternion);
  static void RotateX(Mesh& mesh, float degrees);
  static void RotateY(Mesh& mesh, float degrees);
  static void RotateZ(Mesh& mesh, float degrees);
};

} // end of package
} // end of enterprise

#endif  // GLVIEWER_MESH_MESHTRANSFORM_H
