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
 * A posteffect describes the data of the posteffect.
 * A posteffect can be chained
 */
struct PostEffect {
  MaterialId materialId;
  VertexBufferId vertexBufferId; // TODO: remove this property. Assume always the one from postprocessor.
  std::unordered_map<int, std::string> colorAttachmentMapping;
  std::string depthAttachmentMapping {""};
};

/**
 * Manages the creation of post processors. Also holds a vertex buffer to a full screen quad to facilitate reusing the same vertex buffer.
 */
class PostProcessorManager {
 private:
  static VertexBufferId m_fullscreenQuad;

 public:
  PostProcessorManager(Gpu* m_gpu);

  static VertexBufferId GetFullScreenQuad();

  void createPostProcessor(PostEffect e);

  void createPostProcessor(const std::vector<PostEffect>& e);
};

//class SimplePostprocessor {
// public:
//
//};

/**
 * A postprocessor can render a chain of one or multiple post effects.
 * The postprocessor takes responsibility in using ping-pong buffers. *
 */
class Postprocessor {
 public:
  /**
   * Constructor.
   *
   * @param gpu
   * @param pingPongFramebuffer  Optional. If specified, the postprocessor will use this framebuffer as the ping-pong framebuffer.
   *                             A ping-pong framebuffer is a framebuffer used to chain multiple shader posteffect operations.
   *                             Since the read and draw framebuffer cannot be the same, an alternating pattern need to be used.
   */
  Postprocessor(Gpu* gpu = nullptr, Framebuffer* pingPongFramebuffer = nullptr);

  /**
   * Destructor.
   */
  ~Postprocessor() = default;

//  void setFramebuffer(Framebuffer* framebuffer);
//
//  Framebuffer* getFramebuffer() const;

  /**
   * Adds a posteffect to the chain of posteffects.
   * @param pe The posteffect.
   */
  inline void addPosteffect(PostEffect pe) {
    m_posteffects.push_back(pe);
  }

  inline void setPosteffect(PostEffect pe) {
    m_posteffects.clear();
    addPosteffect(pe);
  }

  /**
   * Renders a posteffect using the currently read framebuffer, outputting it on the write framebuffer.
   * @param material
   */
//  void render(Material& material);
//  void render(MaterialId id);

  /**
   * Raw method used to directly
   * @param from
   * @param to
   * @param material
   */
  void render(Framebuffer* from, Framebuffer* to, PostEffect posteffect);

//  void render(int from, int to, PostEffect);

//  void render(Material& material, GLuint from, GLuint to);
//  void render(MaterialId id, GLuint from, GLuint to);

 private:
  Gpu* m_gpu;
  const Texture* m_pingPongTexture{nullptr};
  Framebuffer* m_pingPongBuffer{nullptr};
  int m_fullScreenQuad;
  int m_width;
  int m_height;

  std::vector<PostEffect> m_posteffects;
};

}
}  // namespace izz
