//
// Created by jlemein on 02-12-21.
//
#pragma once

#include <GL/glew.h>
#include <gl_iframebuffer.h>

namespace izz {
namespace gl {

/**
 * @brief Stores a list of framebuffers.
 * Useful for compositing the framebuffers in a sequence.
 */
class FramebufferSequence {
 public:
};

/**
 * Consists of two framebuffers
 * * fbo: color attachment which is a render buffer
 * * intermediate fbo: with a texture color attachment
 */
class MultipassFramebuffer : public IFramebuffer {
 public:
  /**
   * Constructor.
   * @param width   Width of the framebuffer.
   * @param height  Height of the framebuffer.
   * @param numSamplesMSAA  Preferred number of MSAA samples for the framebuffer.
   */
  MultipassFramebuffer(int width = 800, int height = 600, int numSamplesMSAA = 4);

  virtual ~MultipassFramebuffer() = default;

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
  GLuint m_msFbo{0U}, m_intermediateFbo{0U};

  GLuint m_textureMSAA, m_texture;  // render to texture
  GLuint m_renderbuffer{0U};        // for depth and stencil buffer

  int m_width{800}, m_height{600};
  int m_numSamplesMSAA{8};

  void createMsaaFramebuffer();
  void createIntermediaFbo();
};

}  // namespace gl
}  // namespace izz
