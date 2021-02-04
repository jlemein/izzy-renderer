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
  SceneGraphEntity makePointLight(glm::vec3 intensity = {1.0F, 1.0F, 1.0F},
                        glm::vec3 color = {1.0F, 1.0F, 1.0F},
                        std::string name = "#Light#");

  SceneGraphEntity makeMesh(const geo::Mesh &mesh);
  SceneGraphEntity makeEmptyMesh(const geo::Mesh &mesh);
  SceneGraphEntity makeCurve();

  SceneGraphEntity makeRenderable(geo::Mesh &&mesh, const ecs::Shader &shader);
  SceneGraphEntity makeRenderable(geo::Curve &&curve, const ecs::Shader &shader);

  SceneGraphEntity makeScene(geo::Scene &);
  SceneGraphEntity makeScene(res::Resource<geo::Scene> sceneResource);

  SceneGraphEntity makeTexture();
  SceneGraphEntity makeRectangularGrid(float size = 10.0F, float spacing = 1.0F);
  SceneGraphEntity makeDebugVisualization(entt::entity target);

private:
  /// Uses EnTT in the background for scene management
  entt::registry m_registry;
};

//====================================
// INLINE DEFINITIONS
//====================================

inline entt::registry &SceneGraph::getRegistry() { return m_registry; }

} // namespace ecs
} // namespace affx

#endif // ARTIFAX_ECS_SCENEGRAPH_H
