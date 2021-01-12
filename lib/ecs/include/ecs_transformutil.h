//
// Created by jlemein on 12-11-20.
//

#ifndef ARTIFAX_ECS_TRANSFORMUTIL_H
#define ARTIFAX_ECS_TRANSFORMUTIL_H

#include <glm/glm.hpp>

namespace artifax {
namespace ecs {

struct Transform;

struct TransformUtil {
  //  static void setWorldPosition(Transform& t, glm::vec3& e);
  static void SetPosition(Transform &t, const glm::vec3 &e);

  /// @brief Scales the transformation matrix 'transform' by a given scale
  /// vector 'scale'.
  static void Scale(Transform &transform, const glm::vec3 &scale);
  static void Scale(Transform &transform, float scale);

  static void RotateEuler(Transform& transform, const glm::vec3& axis, float radians);

  /// @brief Translates transformation matrix 'transform' by a 3D translation
  /// vector 't'. Transformation matrix is not reset or reinitialized. It is up
  /// to the user of the function to make sure the operation keeps the transform
  /// in a consistent state.
  static void Translate(Transform &transform, const glm::vec3 &t);

  static void SetWorldPosition(Transform &transform, const glm::vec3 &position);
};

} // namespace ecs
} // namespace artifax

#endif // ARTIFAX_ECS_TRANSFORMUTIL_H
