#ifndef ARTIFAX_ECS_SCENEGRAPH_H
#define ARTIFAX_ECS_SCENEGRAPH_H

#include <ecsg_scenegraphentity.h>

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <memory>

namespace lsw {

namespace geo {
struct Mesh;
struct Curve;
struct Scene;
struct SceneNode;
struct Light;
struct Camera;
struct Material;
};  // namespace geo

namespace ecs {
struct Transform;
struct PointLight;
}  // namespace ecs

namespace ecsg {

/**
 * @brief Specify what scene information to use for scene instantiation.
 * @details It is possible to skip loading
 */
struct SceneLoaderFlags {
  bool animations{true};
  bool cameras{false};
  bool geometry{true};
  bool lights{false};
  bool materials{true};

  static inline SceneLoaderFlags All() {
    return SceneLoaderFlags{true, true, true, true, true};
  }
};

/**!
 * SceneGraph is the central authority to deal with the scene hierarchy.
 * The scene grap deals with storing entities and their relationship.
 *
 */
class SceneGraph {
 public:
  SceneGraph();

  void setDefaultMaterial(std::shared_ptr<geo::Material> material);

  // TODO: represent the active camera in the scene graph,
  //  probably by flagging the entity with ActiveCamera component.
  //  or maybe a centralized registry.
  void setActiveCamera(SceneGraph camera);

  entt::registry& getRegistry();

  /**!
   * @brief Creates a geometry
   * @return
   */
   //TODO: currently assigning a material to a geometry disconnects the relationship with the creator.
   // this does not make it possible to share materials
  SceneGraphEntity addGeometry(geo::Mesh mesh, geo::Material material);
//  SceneGraphEntity addGeometry(geo::Mesh&& mesh, geo::Material&& material);
  SceneGraphEntity addGeometry(const geo::Mesh& mesh);
  SceneGraphEntity addGeometry(geo::Mesh mesh, std::shared_ptr<geo::Material> mat);

      void setActiveCamera(const SceneGraphEntity* activeCamera);

  SceneGraphEntity makeEntity(std::string name = "");

  /// @brief Creates a simple barebone entity containing minimum components
  /// Minum components are: Transform, Name, Relationship
  SceneGraphEntity makeMoveableEntity(std::string name = "");

  /// @brief Creates a camera at a Z distance of 5 meter from the origin,
  /// looking at the origin
  SceneGraphEntity makeCamera(std::string name, float zDistance = 5.0F, float fovx = 120.0F, float aspect = 1.0F, float zNear = 0.1F, float zFar = 1000.0F);

  SceneGraphEntity makeCamera(const geo::Camera& geoCamera);

  SceneGraphEntity makeLight(const geo::Light& light);

  SceneGraphEntity makeAmbientLight(std::string name, glm::vec3 color = {.1F, .1F, .1F}, float intensity = 1.0F);
  SceneGraphEntity makePointLight(std::string name, glm::vec3 position, ecs::PointLight pointLight);
  SceneGraphEntity makePointLight(std::string name, glm::vec3 position);

  /**
   * Creates a directional light source towards the specified direction
   * @param name        [in] A human readable name for the light source.
   * @param direction   [in] The direction of the light source, as seen from the
   *                         origin of the scene.
   * @return A new scene graph entity with the directional light source.
   */
  SceneGraphEntity makeDirectionalLight(std::string name, glm::vec3 direction = {0.10976F, 0.98787834F, 0.10976F});

  SceneGraphEntity makeMesh(const geo::Mesh& mesh);
  SceneGraphEntity makeEmptyMesh(const geo::Mesh& mesh);
  SceneGraphEntity makeCurve(std::string name);

  SceneGraphEntity makeRenderable(geo::Mesh&& mesh, const geo::Material& material);
  SceneGraphEntity makeRenderable(const geo::Mesh& mesh, glm::mat4 transform, geo::Material& material);
  SceneGraphEntity makeRenderable(geo::Curve&& curve, const geo::Material& shader);

  /**!
   * Loads a complete scene and adds it to the scene graph.
   * A scene consists of meshes, a node hierarchy with instances referring to
   * the meshes.
   * Also it consists of cameras, and lights, which can optionally be loaded
   * as well. By default everything is loaded.
   * @return
   */
  SceneGraphEntity makeScene(const geo::Scene&, SceneLoaderFlags flags = SceneLoaderFlags{});

  SceneGraphEntity makeTexture();
  SceneGraphEntity makeRectangularGrid(float size = 10.0F, float spacing = 1.0F);
  SceneGraphEntity makeDebugVisualization(entt::entity target);

  SceneGraphEntity makePosteffect(const std::string name, const geo::Material& material);

 private:
  /// Uses EnTT in the background for scene management
  entt::registry m_registry;
  //  std::shared_ptr<ecsg::EntityFactory> m_entityFactory {nullptr};

  std::shared_ptr<geo::Material> m_defaultMaterial{nullptr};
  const SceneGraphEntity* m_activeCamera{nullptr};

  void processChildren(std::shared_ptr<const geo::SceneNode> node, SceneLoaderFlags flags, SceneGraphEntity* parent_p = nullptr);
};

//====================================
// INLINE DEFINITIONS
//====================================

inline entt::registry& SceneGraph::getRegistry() {
  return m_registry;
}

}  // namespace ecsg
}  // namespace lsw

#endif  // ARTIFAX_ECS_SCENEGRAPH_H
