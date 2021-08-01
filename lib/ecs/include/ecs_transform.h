//
// Created by jlemein on 10-11-20.
//

#ifndef GLVIEWER_ECS_TRANSFORM_H
#define GLVIEWER_ECS_TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace lsw {
namespace ecs {

struct Transform {
  glm::mat4 localTransform  {glm::mat4(1.0F) };

  // derived from local transform, not to be set
  glm::mat4 worldTransform {glm::mat4(1.0F) };
};

//struct Orientation {
//  // default orientation aligned with negative z-axis
//  glm::quat q = glm::angleAxis(0.0F, glm::vec3(0.0F, 0.0F, 1.0F));
//};

//struct Position {
//  glm::vec3 p {0.0F, 0.0F, 0.0F};
//};

} // end package namespace
} // end enterprise namespace

#endif // GLVIEWER_ECS_TRANSFORM_H
