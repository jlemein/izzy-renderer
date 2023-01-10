//
// Created by jlemein on 07-11-20.
//
#pragma once

#include <gl_renderable.h>
#include <memory>
#include <izz_debugsystem.h>

#include <gl_hdrframebuffer.h>
#include <gl_multipassframebuffer.h>
#include <glrs_lightsystem.h>
#include <izzgl_deferredrenderer.h>
#include <izzgl_forwardrenderer.h>
#include <izzgl_meshsystem.h>
#include <izzgl_renderstrategy.h>
#include <entt/fwd.hpp>
#include "izz_texture.h"
#include "izzgl_gammacorrectionpe.h"
#include "izzgl_gpu.h"
#include <izzgl_postprocessor.h>

namespace izz {
namespace gl {
class MaterialSystem;
class TextureSystem;
class SkySystem;
class LightSystem;
class ShaderCompiler;
struct Texture;

/**!
 * Render system that interacts with the GPU using OpenGL.
 */
class RenderSystem : public IRenderCapabilitySelector {
 public:
  static inline const char* ID = "RenderSystem";

  /**
   * Constructor.
   * @param [in] registry        Scenegraph that consists of the entities to be rendered.
   * @param [in] gpu             Access to GPU for buffer structures.
   * @param [in] skySystem       Sky system deals with rendering the atmosphere (skyboxes, etc.).
   * @param [in] lightSystem     Light system.
   */
  RenderSystem(entt::registry& registry,
               std::shared_ptr<izz::gl::Gpu> gpu,
               std::shared_ptr<izz::gl::SkySystem> skySystem,
               std::shared_ptr<izz::gl::LightSystem> lightSystem);

  /**
   * Traverses the scene graph and creates corresponding objects in the render system so that the entities can be rendered.
   */
  void init(int width, int height);
  void update(float dt, float time);
  void render();

  void resize(int width, int height);

  void onGeometryAdded(SceneGraphEntity& e, RenderStrategy renderStrategy);

  /**
   * @returns the light system.
   */
  LightSystem& getLightSystem();

  ~RenderSystem() override = default;

  /// @inherit
  RenderCapabilities selectRenderCapabilities(const MaterialTemplate& materialTemplate) const override;

 private:
  ForwardRenderer m_forwardRenderer;
  DeferredRenderer m_deferredRenderer;
  Postprocessor m_postProcessor;
  glm::vec4 m_clearColor;

  std::unordered_map<TextureId, GLuint> m_allocatedTextures;

  entt::registry& m_registry;
  std::shared_ptr<izz::gl::Gpu> m_gpu{nullptr};
  std::shared_ptr<izz::gl::SkySystem> m_skySystem{nullptr};
  std::shared_ptr<LightSystem> m_lightSystem {nullptr};
  int m_viewportWidth = 0;
  int m_viewportHeight = 0;

  /// Gamma correction posteffect.
  GammaCorrectionPE m_gammaCorrectionPE;

  /// Framebuffer to render
  GLuint m_fbo;
};

}  // namespace gl
}  // namespace izz
