//
// Created by jlemein on 17-11-20.
//

#ifndef GLVIEWER_ECS_FIRSTPERSONCONTROLSYSTEM_H
#define GLVIEWER_ECS_FIRSTPERSONCONTROLSYSTEM_H

#include <entt/fwd.hpp>
#include <glm/glm.hpp>

namespace lsw {
namespace io {
class InputSystem;
}

namespace ecs {
/**
 * @brief Handles input for first person camera behavior. Moving forward, jumping, looking
 * around.
 */
class FirstPersonMovementSystem {
 public:
  FirstPersonMovementSystem(entt::registry& registry, lsw::io::InputSystem* inputSystem);

  void init();
  void update(float dt);

 private:
  entt::registry& m_registry;
  io::InputSystem* m_inputSystem;

  void getRelativeMovement(float& forward, float& right);
};

}  // namespace ecs
}  // namespace lsw

#endif  // GLVIEWER_ECS_FIRSTPERSONCONTROLSYSTEM_H
