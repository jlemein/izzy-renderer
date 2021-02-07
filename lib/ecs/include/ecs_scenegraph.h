#ifndef ARTIFAX_ECS_SCENEGRAPH_H
#define ARTIFAX_ECS_SCENEGRAPH_H

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <ecs_scenegraphentity.h>
#include <res_resource.h>

namespace affx {

namespace geo {
struct Mesh;
struct Curve;
struct Scene;
struct SceneNode;
struct Light;
}; // namespace geo

namespace ecs {
struct Transform;
struct Shader;

/**!
 * SceneGraph is responsible for managing scene node instances.
 *
 */
class SceneGraph {
public:
  SceneGraph() = default;

  entt::registry &getRegistry();

  /// @brief Creates a simple barebone entity containing minimum components
  SceneGraphEntity makeEntity(std::string name = "");

  SceneGraphEntity makeCamera(const glm::mat4 &transform, std::string name = "");
  SceneGraphEntity makeLight(const geo::Light& light);

  SceneGraphEntity makePointLight(glm::vec3 intensity = {1.0F, 1.0F, 1.0F},
                        glm::vec3 color = {1.0F, 1.0F, 1.0F},
                        std::string name = "#Light#");
  SceneGraphEntity makePointLight(std::string name, glm::vec3 position);
  SceneGraphEntity makeDirectionalLight(std::string name, glm::vec3 direction);
  // SceneGraphEntity makeAreaLight(std::string name, SceneGraphEntity lightBody);
  // SceneGraphEntity makeSpotLight(std::string name);

  SceneGraphEntity makeMesh(const geo::Mesh &mesh);
  SceneGraphEntity makeEmptyMesh(const geo::Mesh &mesh);
  SceneGraphEntity makeCurve(std::string name);

  SceneGraphEntity makeRenderable(geo::Mesh &&mesh, const ecs::Shader &shader);
  SceneGraphEntity makeRenderable(const geo::Mesh &mesh, glm::mat4 transform, ecs::Shader&& shader);
  SceneGraphEntity makeRenderable(geo::Curve &&curve, const ecs::Shader &shader);

  /**!
   * Loads a complete scene and adds it to the scene graph.
   * A scene consists of meshes, a node hierarchy with instances referring to
   * the meshes.
   * Also it consists of cameras, and lights, which can optionally be loaded
   * as well. By default everything is loaded.
   * @return
   */
  SceneGraphEntity makeScene(geo::Scene &);
  SceneGraphEntity makeScene(res::Resource<geo::Scene> sceneResource);

  SceneGraphEntity makeTexture();
  SceneGraphEntity makeRectangularGrid(float size = 10.0F, float spacing = 1.0F);
  SceneGraphEntity makeDebugVisualization(entt::entity target);

private:
  /// Uses EnTT in the background for scene management
  entt::registry m_registry;

  void processChildren(std::shared_ptr<const geo::SceneNode> node, SceneGraphEntity* parent_p = nullptr);
};

//====================================
// INLINE DEFINITIONS
//====================================

inline entt::registry &SceneGraph::getRegistry() { return m_registry; }

} // namespace ecs
} // namespace affx

#endif // ARTIFAX_ECS_SCENEGRAPH_H
