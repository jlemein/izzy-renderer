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

  void setFramebufferSize(int width, int height);

  void init();

  void update(float dt);

private:
  int m_bufferWidth {600};
  int m_bufferHeight {400};
  entt::registry &m_registry;
};

} // end of package namespace
} // end of enterprise namespace

#endif // GLVIEWER_ECS_CAMERASYSTEM_H
