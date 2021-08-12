//
// Created by jlemein on 07-11-20.
//

#ifndef GLVIEWER_VIEWER_RENDERSYSTEM_H
#define GLVIEWER_VIEWER_RENDERSYSTEM_H

#include <ecs_debugsystem.h>
#include <ecs_renderable.h>
#include <ecs_rendersubsystem.h>
#include <memory>

#include <glm/glm.hpp>

namespace lsw {

namespace ecsg {
class SceneGraph;
} // namespace ecsg

namespace ecs {
struct Shader;
class TextureSystem;
class IMaterialSystem;
class LightSystem;

/**!
 * Responsible
 */
class RenderSystem {
public:
  RenderSystem(std::shared_ptr<ecsg::SceneGraph> sceneGraph,
               std::shared_ptr<IMaterialSystem> materialSystem);

  void init();
  void update(float time, float dt);
  void render();

  // register as part of the render pipeline
  void addSubsystem(std::shared_ptr<IRenderSubsystem> system);
  void removeSubsystem(std::shared_ptr<IRenderSubsystem> system);

  void setActiveCamera(entt::entity cameraEntity);

  /**
   * Attaches a texture to the renderable unit
   * @param geoTexture
   * @param paramName
   * @return
   */
  void attachTexture(ecs::Renderable &renderable,
                             const geo::Texture &geoTexture,
                             const std::string &paramName);

  void activateTextures(entt::entity e);

private:
  entt::registry &m_registry;
  std::list<std::shared_ptr<IRenderSubsystem>> m_renderSubsystems;
  std::shared_ptr<IMaterialSystem> m_materialSystem;
  ecs::DebugSystem m_debugSystem;
  std::shared_ptr<ecs::LightSystem> m_lightSystem;

  entt::entity m_activeCamera{entt::null};

  /// Lighting parameters needed for the uniform buffer object in the shader.
  /// These lighting properties are for forward lighting (not deferred).
  UniformLighting m_uLightData;
  // UniformDeferredLighting m_uDeferredLightData;

  // groups for performance considerations
  //  entt::group<Renderable> m_renderables;
  //  entt::group<Renderable, Debug> m_debuggables;

  /// makes sure the transformations applied to meshes and cameras are reflected
  /// in the renderable component.
  void synchMvpMatrices();

  /**!
   * @brief Updates the model matrix of the renderable object. After the call
   * the renderable object reflects the current model transformation state.
   * @param renderable
   */
  void updateModelMatrix(entt::entity e);
  void updateCamera(Renderable &renderable);
  void updateLightProperties();

  void initShaderProperties(Renderable& renderable, const geo::Material& material);

  void checkError(entt::entity e);
};

class IMaterialSystem {
public:
  virtual ~IMaterialSystem() = default;
  virtual void synchronizeTextures(RenderSystem& renderSystem) = 0;

  virtual void update(float time, float dt) = 0;

};

} // namespace ecs
} // namespace lsw

#endif // GLVIEWER_VIEWER_RENDERSYSTEM_H
