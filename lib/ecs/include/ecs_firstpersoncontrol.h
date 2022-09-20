#pragma once

namespace izz {
namespace ecs {

struct FirstPersonControl {
  float sensitivityX{1.0F}, sensitivityY{1.0F};
  float defaultVelocity{1.5F};  // 1.5 meters per second, or 5.4 km/h
  float rotationX{0.0F};
  float rotationY{0.0F};

  // if set to false, the mouse motion determines rotation
  // if set to true, rotating is only performed when mouse is clicked.
  bool onlyRotateOnMousePress{false};

  // control mapping
  // ControlMapping
};

}  // namespace ecs
}  // namespace lsw
