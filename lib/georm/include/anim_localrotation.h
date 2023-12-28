//
// Created by jlemein on 09-08-21.
//
#pragma once

#include <glm/glm.hpp>


namespace lsw {
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
