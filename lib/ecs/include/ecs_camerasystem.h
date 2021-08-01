//
// Created by jlemein on 16-11-20.
//

#ifndef GLVIEWER_ECS_CAMERASYSTEM_H
#define GLVIEWER_ECS_CAMERASYSTEM_H

#include <entt/fwd.hpp>

namespace lsw {
namespace ecs {

class CameraSystem {
public:
  CameraSystem(entt::registry &registry);

  void init();

  void update(float dt);

private:
  entt::registry &m_registry;
};

} // end of package namespace
} // end of enterprise namespace

#endif // GLVIEWER_ECS_CAMERASYSTEM_H
