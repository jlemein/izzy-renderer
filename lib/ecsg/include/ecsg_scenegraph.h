#ifndef ARTIFAX_ECS_SCENEGRAPH_H
#define ARTIFAX_ECS_SCENEGRAPH_H

#include <ecsg_scenegraphentity.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <res_resource.h>
#include <memory>

namespace affx {

namespace res {
class ResourceManager;
}

namespace geo {
struct Mesh;
struct Curve;
struct Scene;
struct SceneNode;
struct Light;
struct Camera;
}; // namespace geo

namespace ecs {
struct Transform;
struct Shader;
}

namespace ecsg {

/**!
 * SceneGraph is responsible for managing scene node instances.
 *
 */
class SceneGraph {
public:
  SceneGraph(std::shared_ptr<res::ResourceManager> resourceManager);

  //TODO: represent the active camera in the scene graph,
  // probably by flagging the entity with ActiveCamera component.
  // or maybe a centralized registry.
  void setActiveCamera(SceneGraph camera);

  entt::registry &getRegistry();

  /// @brief Creates a simple barebone entity containing minimum components
  /// Minum components are: Transform, Name, Relationship
  SceneGraphEntity makeEntity(std::string name = "");

  SceneGraphEntity makeCamera(std::string name, float fovx = 120.0F,
                              float aspect = 1.0F, float zNear = 0.1F,
                              float zFar = 1000.0F);

  SceneGraphEntity makeCamera(const geo::Camera &geoCamera);

  SceneGraphEntity makeLight(const geo::Light &light);

  SceneGraphEntity makePointLight(glm::vec3 intensity = {1.0F, 1.0F, 1.0F},
                                  glm::vec3 color = {1.0F, 1.0F, 1.0F},
                                  std::string name = "#Light#");
  SceneGraphEntity makePointLight(std::string name, glm::vec3 position);
  SceneGraphEntity makeDirectionalLight(std::string name, glm::vec3 direction);
  // SceneGraphEntity makeAreaLight(std::string name, SceneGraphEntity
  // lightBody); SceneGraphEntity makeSpotLight(std::string name);

  SceneGraphEntity makeMesh(const geo::Mesh &mesh);
  SceneGraphEntity makeEmptyMesh(const geo::Mesh &mesh);
  SceneGraphEntity makeCurve(std::string name);

  SceneGraphEntity makeRenderable(geo::Mesh &&mesh, const ecs::Shader &shader);
  SceneGraphEntity makeRenderable(const geo::Mesh &mesh, glm::mat4 transform,
                                  ecs::Shader &&shader);
  SceneGraphEntity makeRenderable(geo::Curve &&curve,
                                  const ecs::Shader &shader);

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
  SceneGraphEntity makeRectangularGrid(float size = 10.0F,
                                       float spacing = 1.0F);
  SceneGraphEntity makeDebugVisualization(entt::entity target);


  void setActiveCamera(const SceneGraphEntity* activeCamera);

private:
  /// Uses EnTT in the background for scene management
  entt::registry m_registry;
  std::shared_ptr<res::ResourceManager> m_resourceManager {nullptr};

  const SceneGraphEntity* m_activeCamera {nullptr};

  void processChildren(std::shared_ptr<const geo::SceneNode> node,
                       SceneGraphEntity *parent_p = nullptr);
};

//====================================
// INLINE DEFINITIONS
//====================================

inline entt::registry &SceneGraph::getRegistry() { return m_registry; }

} // namespace ecs
} // namespace affx

#endif // ARTIFAX_ECS_SCENEGRAPH_H
