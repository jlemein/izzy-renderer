//
// Created by jlemein on 09-08-21.
//
#include <glm/glm.hpp>

#ifndef RENDERER_ANIM_LOCALROTATION_H
#define RENDERER_ANIM_LOCALROTATION_H

namespace izz {
namespace anim {

struct LocalRotation {
  /// @brief counter clockwise rotation in radians per seonc
  float radiansPerSecond{.0F};
  float currentRotation {.0F};

  /// @brief the axis of rotation, by default Y axis.
  glm::vec3 axis{0, 1, 0};
};

} // namespace anim
} // namespace lsw

#endif // RENDERER_ANIM_LOCALROTATION_H
