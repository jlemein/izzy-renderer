//
// Created by jlemein on 02-12-21.
//
#pragma once

#include <GL/glew.h>
#include <glrs_iframebuffer.h>

namespace lsw {
namespace glrs {

/**
 * Consists of two framebuffers
 * * fbo: color attachment which is a render buffer
 * * intermediate fbo: with a texture color attachment
 */
class HdrFramebuffer : public IFramebuffer {
 public:
  /**
   * Constructor.
   * @param width   Width of the framebuffer.
   * @param height  Height of the framebuffer.
   * @param numSamplesMSAA  Preferred number of MSAA samples for the framebuffer.
   */
  HdrFramebuffer(int width = 800, int height = 600, int numSamplesMSAA = 4);

  /**
   * Sets the size of the frame buffer. This call should be called before a window context is active.
   * Alternative is providing the dimensions via the constructor.
   * @param [in] width   The width of the framebuffer.
   * @param [in] height  The height of the framebuffer.
   */
  void setSize(int width, int height) override;

  /**
   * @brief Resizes the frame buffer size. Regenerates the underlying buffers to match the specified width and height.
   * Should be called when the window is resized.
   * @param width   Width of the framebuffer.
   * @param height  Height of the framebuffer.
   */
  void resize(int width, int height) override;

  /**
   * Creates the internal framebuffers.
   */
  void initialize() override;

  /**
   * @brief Binds the framebuffer for rendering purposes.
   */
  void bind() override;

  /**
   * @brief Blits the render buffer to texture attachment, ready for a new render pass.
   */
  void nextPass() override;

  /**
   * @brief Blits the render buffer to the default framebuffer to visualize the result
   */
  void apply() override;

 private:
  GLuint m_fbo{0U};   /// @brief initial buffer used to render scene (MSAA, float16)
  GLuint m_fbo1 {0U}; /// @brief postprocessed buffer with bright spots
  GLuint m_fbo2 {0U};

  GLuint m_hdrTexture, m_intermediateHdr; // render to texture
  GLuint m_renderbuffer {0U}; // for depth and stencil buffer

  GLuint m_vertexAttrib {0};    // 0 is the index in the vertex shader for position data.
  GLuint m_uvAttrib {1};        // 1 is the index in the vertex shader for uv coordinates.

  int m_width {800}, m_height{600};
  int m_numSamplesMSAA {8};
};

}  // namespace glrs
}  // namespace lsw
