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
  GLuint m_framebuffer;
  GLuint m_vbo, m_vao;
  GLuint m_program;
  GLuint m_texture;

  GLuint m_vertexAttrib {0};
  GLuint m_uvAttrib {1};
};

}  // namespace glrs
}  // namespace lsw
