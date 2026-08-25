//
// Created by jeffrey on 10-1-23.
//
#pragma once

#include <unordered_map>

namespace izz {
namespace gl {

class Framebuffer;
class Texture;
class Gpu;

class FramebufferBuilder {
 public:
  FramebufferBuilder(izz::gl::Gpu& gpu);

  FramebufferBuilder& setColorAttachment(Texture* texture);
  FramebufferBuilder& setColorAttachment(int attachment, Texture* texture);
  FramebufferBuilder& setDepthAttachment(Texture* texture);

  Framebuffer* create();

 private:
  izz::gl::Gpu& m_gpu;

  std::unordered_map<int, Texture*> m_colorAttachments;
  Texture* m_depthAttachment{nullptr};
};

}  // namespace gl
}  // namespace izz
