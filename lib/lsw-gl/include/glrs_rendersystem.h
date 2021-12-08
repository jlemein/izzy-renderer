//
// Created by jlemein on 07-11-20.
//
#pragma once

#include <ecs_debugsystem.h>
#include <glrs_renderable.h>
#include <memory>

#include <glrs_lightsystem.h>
#include "glrs_multipassframebuffer.h"

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

//  void setFramebuffer()
  MultipassFramebuffer& getFramebuffer();

  /**
   * @returns the light system.
   */
  glrs::LightSystem& getLightSystem();

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
  // framebuffer mechanism
  MultipassFramebuffer m_framebuffer;

  entt::registry& m_registry;
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

  /**
   * @brief Sets up the render component (i.e. the handle to the render system)
   * with the assigned material properties for this entity. Every material has a
   * uniform properties attribute that gets filled based on ....
   *
   * @details
   * The material component (@see geo::Material) contains a set of attributes,
   * easily editable in the code. Eventually the attributes gets mapped to a
   * uniform property attribute.
   *
   * @param renderable The render component
   * @param properties The material properties.
   */
  void initShaderProperties(entt::entity entity, Renderable& renderable, const geo::Material& material);

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
