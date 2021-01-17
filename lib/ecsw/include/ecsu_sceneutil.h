//
// Created by jlemein on 13-01-21.
//

#ifndef GLVIEWER_ECSU_SCENEUTIL_H
#define GLVIEWER_ECSU_SCENEUTIL_H

#include <entt/fwd.hpp>

namespace artifax {
namespace ecsw {
class Scene;
}

namespace ecsu {
/**
 * All utility methods for creation of scene graph entities
 */
class SceneUtil {
public:
  /**!
   * Adds a scene object to the registry.
   * @param registry The registry to add the scene contents too
   */
  static void addToScene(entt::registry& registry, ecsw::Scene& model);

  static void makeLight();
  static void makePointLight();
  static void makeDirectionalLight();
//  static void makeAmbientLight(glm::vec3 color, float intensity = 1.0F);
//  static void makeAreaLight(entt::registry&, ecs::Mesh& mesh);

  static entt::entity makeEntity();
};

} // end of package
} // end of enterprise

#endif // GLVIEWER_ECSU_SCENEUTIL_H
