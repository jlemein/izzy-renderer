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

namespace izz {
namespace gl {
class MaterialSystem;
class TextureSystem;
class LightSystem;
class ShaderCompiler;
struct Texture;

/**!
 * Render system that interacts with the GPU using OpenGL.
 */
class SkySystem {
 public:
  static inline const char* ID = "SkySystem";

  /**
   * Constructor.
   * @param [in] registry        Scenegraph that consists of the entities to be rendered.
   * @param [in] materialSystem  Material system deals with updating and gathering of material properties.
   * @param [in] textureSystem   Texture system deals with creation of texture buffers.
   * @param [in] meshSystem      Mesh system to allocate vertex and index buffers for mesh data.
   * @param [in] lightSystem     Light system.
   */
  SkySystem(entt::registry& registry,
               std::shared_ptr<izz::gl::MaterialSystem> materialSystem,
               std::shared_ptr<TextureSystem> textureSystem,
               std::shared_ptr<izz::gl::MeshSystem> meshSystem);

  virtual ~SkySystem() = default;

  /**
   * Traverses the scene graph and creates corresponding objects in the render system so that the entities can be rendered.
   */
  void init(int width, int height);
  void update(float dt, float time);

  void render();

  /**
   * Sets the environment map.
   */
  void setEnvironmentMap(Texture* environmentMap, Texture* convolutedMap);

  /**
   * Updates the materials with the environment maps.
   *
   * @details
   * Many materials require environment maps to show reflections or use them for lighting the scene. Depending on the use of environment maps, they may
   * need to be set once for each material, or the map get's recomputed per frame to deal with dynamic objects.
   * Calling this method loops through the materials and updates the environment map texture.
   */
  void updateEnvironmentMaps();

  void resize(int width, int height);

 private:
  entt::registry& m_registry;
  std::shared_ptr<izz::gl::MaterialSystem> m_materialSystem{nullptr};
  std::shared_ptr<izz::gl::MeshSystem> m_meshSystem{nullptr};
  std::shared_ptr<izz::gl::TextureSystem> m_textureSystem{nullptr};
  std::shared_ptr<LightSystem> m_lightSystem;
  int m_width = 0;
  int m_height = 0;

  VertexBufferId m_unitCubeVertexBufferId = {-1}; /// unit cube vertex buffer
  GLuint m_fbo; /// Framebuffer to render

  /**
   * Event handler that is triggered whenever a Skybox is created or reassigned to an entity.
   * @param registry
   * @param e
   */
  void onSkyboxModified(entt::registry& registry, entt::entity e);
};

}  // namespace gl
}  // namespace izz
