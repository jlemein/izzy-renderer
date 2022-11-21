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

namespace izz {
class ResourceManager;
namespace gl {
class MaterialSystem;
class TextureSystem;
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
   * @param [in] sceneGraph      Scenegraph that consists of the entities to be rendered.
   * @param [in] materialSystem  Material system deals with updating and gathering of material properties.
   * @param [in] meshSystem      Mesh system to allocate vertex and index buffers for mesh data.
   */
  RenderSystem(entt::registry& registry, std::shared_ptr<izz::ResourceManager> resourceManager, std::shared_ptr<izz::gl::MaterialSystem> materialSystem,
               std::shared_ptr<TextureSystem> textureSystem,
               std::shared_ptr<izz::gl::MeshSystem> meshSystem,
               std::shared_ptr<izz::gl::LightSystem> lightSystem);

  /**
   * Traverses the scene graph and creates corresponding objects in the render system so that the entities can be rendered.
   */
  void init(int width, int height);
  void update(float dt, float time);
  void render();

  void renderSkybox();

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
  glm::vec4 m_clearColor;

  std::unordered_map<TextureId, GLuint> m_allocatedTextures;
  VertexBufferId m_unitCubeVertexBufferId = {-1};

  entt::registry& m_registry;
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
