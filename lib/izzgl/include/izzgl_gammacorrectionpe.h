//
// Created by jlemein on 07-11-20.
//
#pragma once

#include <gl_renderable.h>
#include <memory>

#include <gl_hdrframebuffer.h>
#include <gl_multipassframebuffer.h>
#include <izzgl_meshsystem.h>
#include <izzgl_renderstrategy.h>
#include <entt/fwd.hpp>
#include <izzgl_postprocessor.h>

namespace izz {
namespace gl {
class Gpu;
class Framebuffer;
class Postprocessor;

struct Texture;

/**!
 * Gamma correction is a posteffect
 */
class GammaCorrectionPE {
 public:
  static inline const char* ID = "GammaCorrectionPE";

  /**
   * Constructor.
   * @param [in] gpu             Access to GPU.
   * @param [in] registry        Scenegraph that consists of the entities to be rendered.
   */
  GammaCorrectionPE(std::shared_ptr<Gpu> gpu, Postprocessor& postprocesor, entt::registry& registry);

  virtual ~GammaCorrectionPE() = default;

  /**
   * Traverses the scene graph and creates corresponding objects in the render system so that the entities can be rendered.
   */
  void init(int width, int height);
  void update(float dt, float time);
  void render();

  /**
   * Called whenever a resize operation is performed. Offers ability for posteffect to resize frame buffer window sizes.
   * @param width
   * @param height
   */
  void resize(int width, int height);


 private:
  std::shared_ptr<izz::gl::Gpu> m_gpu{nullptr};
  entt::registry& m_registry;

  int m_width = 0;
  int m_height = 0;

  /// Framebuffer to render
  MaterialId m_postEffect{-1};
//  VertexBufferId m_rectangle{-1};
  Texture* m_hdrTexture{nullptr};
  Framebuffer* m_framebuffer {nullptr};
  Postprocessor& m_postprocessor;
  Postprocessor m_posteffects;
  GLuint m_fbo;
};

}  // namespace gl
}  // namespace izz
