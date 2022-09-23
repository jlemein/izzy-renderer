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
class EffectSystem;
class MaterialSystem;
class LightSystem;
class ShaderCompiler;
struct Texture;


enum class RenderStrategy {
  UNDEFINED = 0,
  FORWARD,
  DEFERRED
};

/**!
 * Render system that interacts with the GPU using OpenGL.
 */
class RenderSystem {
 public:
  static inline const char* ID = "RenderSystem";

  /**
   * Constructor.
   * @param [in] sceneGraph      Scenegraph that consists of the entities to be rendered.
   * @param [in] materialSystem  Material system deals with updating and gathering of material properties.
   * @param [in] meshSystem      Mesh system to allocate vertex and index buffers for mesh data.
   */
  RenderSystem(entt::registry& registry,
               std::shared_ptr<izz::ResourceManager> resourceManager,
               std::shared_ptr<izz::gl::MaterialSystem> materialSystem,
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

 private:
  ForwardRenderer m_forwardRenderer;
  DeferredRenderer m_deferredRenderer;

  std::unordered_map<TextureId, GLuint> m_allocatedTextures;

  entt::registry& m_registry;
  std::shared_ptr<ShaderCompiler> m_shaderSystem;
  std::shared_ptr<izz::ResourceManager> m_resourceManager {nullptr};
  std::shared_ptr<izz::gl::MaterialSystem> m_materialSystem {nullptr};
  std::shared_ptr<izz::gl::MeshSystem> m_meshSystem {nullptr};
//  lsw::ecs::DebugSystem m_debugSystem;
  std::shared_ptr<LightSystem> m_lightSystem;
  int m_viewportWidth = 0;
  int m_viewportHeight = 0;

  GLuint m_quadVbo, m_quadVao;

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
  void initShaderProperties(entt::entity entity, Renderable& renderable, const Material& material);

  /**
   * Unscoped shader properties are global uniform variables in the material. They are not placed in an
   * interface block. Therefore, each of them are queried the location and stored in an efficient
   * data structure as part of the Renderable component.
   * @param renderable  Renderable component
   * @param material    Material containing the unscoped shader properties.
   */
//  void initUnscopedShaderProperties(entt::entity entity, Renderable& renderable, const Material& material);

  void checkError(entt::entity e);

  void initPostprocessBuffers();
//  void renderPosteffects();
};

}  // namespace glrs
}  // namespace lsw
