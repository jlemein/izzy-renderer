//
// Created by jlemein on 01-12-20.
//

#ifndef ARTIFAX_ECS_DEBUGSYSTEM_H
#define ARTIFAX_ECS_DEBUGSYSTEM_H

#include <ecs_debug.h>
#include <glrs_renderable.h>
#include <entt/entity/registry.hpp>
#include <geo_curve.h>
#include <geo_mesh.h>

#include "ecs_debugmodel.h"
#include <unordered_map>

namespace lsw {
namespace ecs {

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
};

} // namespace ecs
} // namespace lsw

#endif // ARTIFAX_ECS_DEBUGSYSTEM_H
