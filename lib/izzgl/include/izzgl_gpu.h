//
// Created by jeffrey on 4-1-23.
//
#pragma once

#include <list>
#include <unordered_map>

namespace izz {
namespace gl {

class MaterialSystem;
class TextureSystem;
class MeshSystem;
class Gpu;
class Texture;
class Framebuffer;

/**
 * Gpu provides access to buffer structures on the GPU. It wraps the lower level material, texture and mesh system to provide a
 * unified interface to the GPU (memory).
 */
class Gpu {
 public:
  Gpu(izz::gl::MaterialSystem& materialSystem, izz::gl::TextureSystem& textureSystem, izz::gl::MeshSystem& meshSystem);

  ~Gpu();

 public:
  izz::gl::MaterialSystem& materials;
  izz::gl::TextureSystem& textures;
  izz::gl::MeshSystem& meshes;

  inline Framebuffer* getDefaultFramebuffer() {
    return m_defaultFramebuffer;
  }

  /**
   * Generates a framebuffer object with valid id.
   * @return
   */
  Framebuffer* allocateFramebuffer();

  /**
   * Binds the frame buffer. It binds both the read and draw framebuffers.
   * @param framebuffer
   */
  void bindFramebuffer(Framebuffer* framebuffer);

  /**
   *
   * @param framebuffer
   */
  void bindReadFramebuffer(Framebuffer* framebuffer);

  /**
   *
   * @param framebuffer
   */
  void bindDrawFramebuffer(Framebuffer* framebuffer);

  inline Framebuffer* getActiveFramebuffer() {
    return m_activeFramebuffer;
  }

 private:
  std::list<izz::gl::Framebuffer> m_framebuffers;
  Framebuffer* m_activeFramebuffer{nullptr};
  Framebuffer* m_defaultFramebuffer;
};

}  // namespace gl
}  // namespace izz
