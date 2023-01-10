//
// Created by jeffrey on 6-1-23.
//
#pragma once

#include <cstdint>
#include <unordered_map>

namespace izz {
namespace gl {

class Gpu;
class Texture;

class Framebuffer {
 public:
  /**
   *
   * @param gpu
   * @param handle The id of the frame buffer, in case of opengl returned using glGenFramebuffers().
   */
  Framebuffer(izz::gl::Gpu& gpu, uint32_t bufferId = 0);

  Framebuffer& operator=(const Framebuffer& framebuffer);

  /**
   * Binds the framebuffer, thereby making it the active framebuffer
   */
  void bind();
  void bindAsRead();
  void bindAsDestination();

  uint32_t bufferId();

  inline Texture* getColorAttachment(int n) const {
    return m_colorAttachments.contains(n) ? m_colorAttachments.at(n) : nullptr;
  }

  inline Texture* getDepthAttachment() const {
    return m_depthAttachment;
  }

  inline void setColorAttachment(int n, Texture* texture) {
    m_colorAttachments[n] = texture;
  }

  inline void setDepthAttachment(Texture* texture) {
    m_depthAttachment = texture;
  }

 private:
  izz::gl::Gpu& m_gpu;
  uint32_t m_fbo {0};

  std::unordered_map<int, Texture*> m_colorAttachments;
  Texture* m_depthAttachment{nullptr};
};

}  // namespace gl
}  // namespace izz
