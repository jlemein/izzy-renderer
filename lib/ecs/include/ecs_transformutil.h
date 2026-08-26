//
// Created by jlemein on 12-11-20.
//
#pragma once

#include <glm/glm.hpp>
#include "izz_scenegraphentity.h"

namespace izz {
namespace ecs {

struct Transform;

struct TransformUtil {
  //  static void setWorldPosition(Transform& t, glm::vec3& e);
  static void SetPosition(Transform& t, const glm::vec3& e);

  /// @brief Scales the transformation matrix 'transform' by a given scale
  /// vector 'scale'.
  static void Scale(Transform& transform, const glm::vec3& scale);
  static void Scale(Transform& transform, float scale);
  static void Scale(glm::mat4& transform, float scale);
  static glm::mat4 Scale(const glm::mat4& transform, float scale);

  static void RotateEuler(Transform& transform, const glm::vec3& axis, float radians);
  static glm::mat4 RotateEulerRadians(const glm::vec3& axis, float radians);
  static glm::mat4 RotateEulerDegrees(const glm::vec3& axis, float degrees);

  /// @brief Translates transformation matrix 'transform' by a 3D translation
  /// vector 't'. Transformation matrix is not reset or reinitialized. It is up
  /// to the user of the function to make sure the operation keeps the transform
  /// in a consistent state.
  static void Translate(Transform& transform, const glm::vec3& t);
  static void Translate(glm::mat4& transform, const glm::vec3& t);

  static void SetWorldPosition(Transform& transform, const glm::vec3& position);

  static void UpdateTransformChain(izz::SceneGraphEntity e);
};

}  // namespace ecs
}  // namespace izz
