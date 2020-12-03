//
// Created by jlemein on 01-12-20.
//

#ifndef ARTIFAX_ECS_DEBUGSYSTEM_H
#define ARTIFAX_ECS_DEBUGSYSTEM_H

#include <entt/entity/registry.hpp>
#include <ecs_debug.h>
#include <ecs_renderable.h>

#include <unordered_map>

namespace artifax
{
namespace ecs
{

/**!
 * @brief Responsible for showing debug shapes for renderable objects
 */
class DebugSystem
{
 public:
  DebugSystem(entt::registry &registry);

  void init();

  void preloadDebugAssets();

  void update(float dt);

 private:
  entt::registry &m_registry;
  entt::entity m_eulerAngleShape;
  std::unordered_map<DebugShape, Renderable> m_debugShapeRenderables;
};

} // end of package
} // end of enterprise

#endif  // ARTIFAX_ECS_DEBUGSYSTEM_H
