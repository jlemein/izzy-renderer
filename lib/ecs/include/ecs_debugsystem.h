//
// Created by jlemein on 01-12-20.
//

#ifndef ARTIFAX_ECS_DEBUGSYSTEM_H
#define ARTIFAX_ECS_DEBUGSYSTEM_H

#include <ecs_debug.h>
#include <ecs_renderable.h>
#include <entt/entity/registry.hpp>
#include <shp_curve.h>
#include <shp_mesh.h>

#include <unordered_map>

namespace artifax {
namespace ecs {

struct DebugPrefab {
  Renderable renderable;
  Shader shader;
  shp::Mesh mesh;
//  shp::Curve curve;
};

/**!
 * @brief Responsible for showing debug shapes for renderable objects
 */
class DebugSystem {
public:
  DebugSystem(entt::registry &registry);

  void init();

  void update(float dt);

private:
  entt::registry &m_registry;
  entt::entity m_eulerAngleShape;
  std::unordered_map<DebugShape, DebugPrefab> m_debugPrefabs;
};

} // namespace ecs
} // namespace artifax

#endif // ARTIFAX_ECS_DEBUGSYSTEM_H
