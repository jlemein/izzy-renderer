//
// Created by jlemein on 07-11-20.
//
#pragma once

#include <ecs_debugsystem.h>
#include <glrs_renderable.h>
#include <memory>

#include <glrs_lightsystem.h>
#include <glrs_multipassframebuffer.h>
#include "glrs_hdrframebuffer.h"
#include <gl_forwardrenderer.h>
#include <gl_deferredrenderer.h>

namespace izz {
class SceneGraph;
namespace gl {
class EffectSystem;
class MaterialSystem;
}
}
namespace lsw {

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
  RenderSystem(std::shared_ptr<izz::SceneGraph> sceneGraph, std::shared_ptr<glrs::IMaterialSystem> materialSystem,
               std::shared_ptr<izz::gl::EffectSystem> effectSystem);

  /**
   * Traverses the scene graph and creates corresponding objects in the render system so that the entities can be rendered.
   */
  void init();
  void update(float time, float dt);
  void render();

  IFramebuffer& getFramebuffer();

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

  /// @brief Activates the effect. Sets up framebuffer.
  void activateEffect(entt::entity e);

  void activateTextures(entt::entity e);

 private:
  std::unique_ptr<IFramebuffer> m_framebuffer;

  izz::gl::ForwardRenderer m_forwardRenderer;
  izz::gl::DeferredRenderer m_deferredRenderer;

  entt::registry& m_registry;
  std::shared_ptr<ShaderSystem> m_shaderSystem;
  std::shared_ptr<IMaterialSystem> m_materialSystem;
  std::shared_ptr<izz::gl::EffectSystem> m_effectSystem;
  ecs::DebugSystem m_debugSystem;
  std::shared_ptr<LightSystem> m_lightSystem;

  entt::entity m_activeCamera{entt::null};

  GLuint m_quadVbo, m_quadVao;

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

  /**
   * Unscoped shader properties are global uniform variables in the material. They are not placed in an
   * interface block. Therefore, each of them are queried the location and stored in an efficient
   * data structure as part of the Renderable component.
   * @param renderable  Renderable component
   * @param material    Material containing the unscoped shader properties.
   */
  void initUnscopedShaderProperties(entt::entity entity, Renderable& renderable, const geo::Material& material);

  void checkError(entt::entity e);

  void initPostprocessBuffers();
  void renderPosteffects();
};

class IMaterialSystem {
 public:
  virtual ~IMaterialSystem() = default;
  virtual void synchronizeTextures(RenderSystem& renderSystem) = 0;

  virtual void update(float time, float dt) = 0;
};

}  // namespace glrs
}  // namespace lsw
