//
// Created by jlemein on 02-12-21.
//
#pragma once

#include <GL/glew.h>

namespace lsw {
namespace glrs {

class MultipassFramebuffer {
 public:
  MultipassFramebuffer();

  /**
   * @brief Updates the frame buffer size. Regenerates the underlying buffers to match the specified width and height.
   * @param width
   * @param height
   */
  void resize(int width, int height);

  /**
   * Initializes the framebuffer and creates buffers needed to apply post processing effects.
   */
  void initialize();

  /**
   * Binds the framebuffer
   */
  void bindFramebuffer();

  /**
   * Renders the framebuffer
   */
  void finish();

 private:
  GLuint m_msFbo{0U}, m_intermediateFbo {0U};
  GLuint m_textureMSAA, m_texture; // render to texture
  GLuint m_renderbuffer {0U}; // for depth and stencil buffer

  GLuint m_vbo {0U}, m_vao {0U};
  GLuint m_program {0U};

  GLuint m_vertexAttrib {0};    // 0 is the index in the vertex shader for position data.
  GLuint m_uvAttrib {1};        // 1 is the index in the vertex shader for uv coordinates.

  int m_numSamplesMSAA {8};

  int m_width {800}, m_height{600};

  void createMsaaFramebuffer();
  void createIntermediaFbo();
};

}  // namespace glrs
}  // namespace lsw
