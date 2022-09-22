//
// Created by jlemein on 17-11-20.
//

#ifndef GLVIEWER_ECS_MOVEABLE_H
#define GLVIEWER_ECS_MOVEABLE_H

#include <glm/vec3.hpp>
namespace izz {
namespace ecs {

struct Moveable {
  float velocity;

  /// @brief acceleration used to increase the velocity per time frame
  float acceleration;
};

} // end package
} // end enterprise

#endif // GLVIEWER_ECS_MOVEABLE_H
