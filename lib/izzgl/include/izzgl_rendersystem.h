//
// Created by jlemein on 07-11-20.
//
#pragma once

#include <ecs_debugsystem.h>
#include <gl_renderable.h>
#include <memory>

#include <gl_deferredrenderer.h>
#include <gl_forwardrenderer.h>
#include <gl_multipassframebuffer.h>
#include <glrs_lightsystem.h>
#include <entt/fwd.hpp>
#include "gl_hdrframebuffer.h"
#include "izzgl_meshsystem.h"
#include "izzgl_texture.h"

namespace izz {
class ResourceManager;
namespace gl {
class MaterialSystem;
class TextureSystem;
class LightSystem;
class ShaderCompiler;
struct Texture;

enum class RenderStrategy { UNDEFINED = 0, FORWARD, DEFERRED };

/**!
 * Render system that interacts with the GPU using OpenGL.
 */
class RenderSystem : public IRenderCapabilitySelector {
 public:
  static inline const char* ID = "RenderSystem";

  /**
   * Constructor.
   * @param [in] sceneGraph      Scenegraph that consists of the entities to be rendered.
   * @param [in] materialSystem  Material system deals with updating and gathering of material properties.
   * @param [in] meshSystem      Mesh system to allocate vertex and index buffers for mesh data.
   */
  RenderSystem(entt::registry& registry, std::shared_ptr<izz::ResourceManager> resourceManager, std::shared_ptr<izz::gl::MaterialSystem> materialSystem,
               std::shared_ptr<TextureSystem> textureSystem,
               std::shared_ptr<izz::gl::MeshSystem> meshSystem);

  /**
   * Traverses the scene graph and creates corresponding objects in the render system so that the entities can be rendered.
   */
  void init(int width, int height);
  void update(float dt, float time);
  void render();

  void resize(int width, int height);

  void addRenderableComponent(SceneGraphEntity& e, RenderStrategy renderStrategy);

  /**
   * @returns the light system.
   */
  LightSystem& getLightSystem();

  ~RenderSystem() override = default;

  /// @inherit
  RenderCapabilities selectRenderCapabilities(const geo::MaterialTemplate& materialTemplate) const override;

 private:
  ForwardRenderer m_forwardRenderer;
  DeferredRenderer m_deferredRenderer;
  glm::vec4 m_clearColor;

  std::unordered_map<TextureId, GLuint> m_allocatedTextures;

  entt::registry& m_registry;
  std::shared_ptr<ShaderCompiler> m_shaderSystem;
  std::shared_ptr<izz::ResourceManager> m_resourceManager{nullptr};
  std::shared_ptr<izz::gl::MaterialSystem> m_materialSystem{nullptr};
  std::shared_ptr<izz::gl::MeshSystem> m_meshSystem{nullptr};
  //  lsw::ecs::DebugSystem m_debugSystem;
  std::shared_ptr<LightSystem> m_lightSystem;
  int m_viewportWidth = 0;
  int m_viewportHeight = 0;

  GLuint m_quadVbo, m_quadVao;

  void initPostprocessBuffers();
  //  void renderPosteffects();
};

}  // namespace gl
}  // namespace izz
