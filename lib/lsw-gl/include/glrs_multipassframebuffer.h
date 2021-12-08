//
// Created by jlemein on 02-12-21.
//
#pragma once

#include <GL/glew.h>

namespace lsw {
namespace glrs {

/**
 * Consists of two framebuffers
 * * fbo: color attachment which is a render buffer
 * * intermediate fbo: with a texture color attachment
 */
class MultipassFramebuffer {
 public:
  /**
   * Constructor.
   * @param numSamplesMSAA  Preferred number of MSAA samples for the framebuffer.
   */
  MultipassFramebuffer(int width = 800, int height = 600, int numSamplesMSAA = 4);

  void setSize(int width, int height);

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
   * Makes sure the result of the previous output is bound as a texture target.
   */
  void nextPass();

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

  int m_width {800}, m_height{600};
  int m_numSamplesMSAA {8};

  void createMsaaFramebuffer();
  void createIntermediaFbo();
};

}  // namespace glrs
}  // namespace lsw
