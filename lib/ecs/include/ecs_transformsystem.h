//
// Created by jlemein on 10-11-20.
//

#ifndef GLVIEWER_ECS_TRANSFORMSYSTEM_H
#define GLVIEWER_ECS_TRANSFORMSYSTEM_H

#include <entt/entt.hpp>

namespace izz {
namespace ecs {

class TransformSystem {
public:
  TransformSystem(entt::registry& registry);

  void init();
  void update(float time, float dt);
  void render();

  entt::registry& m_registry;
};

} // end package namespace
} // end enterprise namesapce

#endif // GLVIEWER_ECS_TRANSFORMSYSTEM_H
