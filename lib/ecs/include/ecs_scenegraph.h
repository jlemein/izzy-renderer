//
// Created by jlemein on 13-01-21.
//

#ifndef ARTIFAX_ECS_SCENEGRAPH_H
#define ARTIFAX_ECS_SCENEGRAPH_H

#include <entt/entt.hpp>

/**!
 * SceneGraph is responsible for managing scene node instances.
 *
 */
class SceneGraph {
public:
  SceneGraph() = default;

  /**!
   * Creates a standard entity with a position,
   * @return
   */
  SceneGraphEntity makeEntity();


private:
  /// Uses EnTT in the background for scene management
  entt::registry m_registry;
};
#endif // ARTIFAX_ECS_SCENEGRAPH_H
