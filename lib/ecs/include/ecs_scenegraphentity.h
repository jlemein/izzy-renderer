//
// Created by jlemein on 13-01-21.
//

#ifndef GLVIEWER_ECS_SCENEGRAPHENTITY_H
#define GLVIEWER_ECS_SCENEGRAPHENTITY_H

#include <ecs_scenegraph.h>
#include <memory>
#include <entt/entt.h>

namespace affx {
namespace ecs {

class SceneGraphEntity {
public:
  template <typename T>
  T& getComponent<T>() {
    m_registry.lock();
    m_registry->
  }

private:
  std::weak_ptr<entt::registry> m_registry;
};

}
}
#endif // GLVIEWER_ECS_SCENEGRAPHENTITY_H
