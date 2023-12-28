//
// Created by jlemein on 07-11-20.
//
#pragma once

#include <ecs_debugsystem.h>
#include <glrs_renderable.h>
#include <glrs_rendersubsystem.h>
#include <memory>

#include <glrs_lightsystem.h>

namespace lsw {

namespace ecsg {
class SceneGraph;
}  // namespace ecsg

namespace glrs {
class IMaterialSystem;
class LightSystem;
class ShaderSystem;

/**!
 * Render system that interacts with the GPU using OpenGL.
 */
class RenderSystem {
 public:
  /**
   * Constructor.
   * @param [in] sceneGraph      Scenegraph that consists of the entities to be rendered.
   * @param [in] materialSystem  Material system deals with updating and gathering of material properties.
   */
  RenderSystem(std::shared_ptr<ecsg::SceneGraph> sceneGraph, std::shared_ptr<glrs::IMaterialSystem> materialSystem);

  /**
   * Traverses the scene graph and creates corresponding objects in the render system so that the entities can be rendered.
   */
  void init();
  void update(float time, float dt);
  void render();

  /**
   * @returns the light system.
   */
  glrs::LightSystem& getLightSystem();

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
  void attachTexture(glrs::Renderable& renderable, const geo::Texture& geoTexture, const std::string& paramName);

  void activateTextures(entt::entity e);

 private:
  entt::registry& m_registry;
  std::list<std::shared_ptr<IRenderSubsystem>> m_renderSubsystems;
  std::shared_ptr<ShaderSystem> m_shaderSystem;
  std::shared_ptr<IMaterialSystem> m_materialSystem;
  ecs::DebugSystem m_debugSystem;
  std::shared_ptr<LightSystem> m_lightSystem;

  entt::entity m_activeCamera{entt::null};

  /// makes sure the transformations applied to meshes and cameras are reflected
  /// in the renderable component.
  void synchMvpMatrices();

  /**!
   * @brief Updates the model matrix of the renderable object. After the call
   * the renderable object reflects the current model transformation state.
   * @param renderable
   */
  void updateModelMatrix(entt::entity e);
  void updateCamera(Renderable& renderable);

  void initShaderProperties(Renderable& renderable, const geo::Material& material);

  void checkError(entt::entity e);
};

class IMaterialSystem {
 public:
  virtual ~IMaterialSystem() = default;
  virtual void synchronizeTextures(RenderSystem& renderSystem) = 0;

  virtual void update(float time, float dt) = 0;
};

}  // namespace glrs
}  // namespace lsw
