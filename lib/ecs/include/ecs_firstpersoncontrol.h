//
// Created by jlemein on 17-11-20.
//

#ifndef GLVIEWER_ECS_FIRSTPERSONCONTROL_H
#define GLVIEWER_ECS_FIRSTPERSONCONTROL_H

namespace lsw {
namespace ecs {

struct FirstPersonControl {
  float sensitivityX {1.0F}, sensitivityY {1.0F};

  float defaultVelocity {1.5F};   // 1.5 meters per second, or 5.4 km/h

  float rotationX {0.0F};
  float rotationY {0.0F};

  // control mapping
  // ControlMapping
};

} // namespace package
} // namespace enterprise

#endif // GLVIEWER_ECS_FIRSTPERSONCONTROL_H
