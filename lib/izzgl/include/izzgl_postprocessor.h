//
// Created by jeffrey on 6-1-23.
//
#pragma once

#include <GL/glew.h>
#include <izzgl_framebuffer.h>

namespace izz {
namespace gl {

class Gpu;
class Material;
//class Framebuffer;

/**
 * Easy class to perform postprocessing operations on framebuffers.
 */
class Postprocessor {
 public:
  /**
   * @details
   * When performing postprocessing a frame
   * @param gpu
   * @param framebuffer  Temporary framebuffer to copyUsed for post processing effects.
   */
  Postprocessor(Gpu& gpu, Framebuffer* framebuffer);
  ~Postprocessor() = default;

  /**
   * Initializes the buffers needed to perform render operations.
   */
  void initialize();

  void setFramebuffer(Framebuffer* framebuffer);

  Framebuffer* getFramebuffer() const;

  /**
   * Renders a posteffect using the currently read framebuffer, outputting it on the write framebuffer.
   * @param material
   */
  void render(Material& material);
  void render(MaterialId id);

  void render(Material& material, GLuint from, GLuint to);
  void render(MaterialId id, GLuint from, GLuint to);

 private:
  Gpu& m_gpu;
  const Texture* m_copyTexture {nullptr};
  Framebuffer* m_tempBuffer {nullptr};
  int m_screenRect;
  int m_width;
  int m_height;
};

}
}  // namespace izz
