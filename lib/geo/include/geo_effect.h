//
// Created by jeffrey on 16-12-21.
//
#pragma once

#include <geo_material.h>
#include <geo_graph.h>
#include <memory>


namespace izz {
namespace geo {

// input buffers
// - color

enum class ColorBuffer { UNUSED, CUBEMAP, BUFFER_1D, BUFFER_2D, TEXTURE_2D, TEXTURE_2D_MULTISAMPLE };

// there are two ways to specify a target
// - texture targets (are readable)
// - renderbuffer targets (are not readable).
// both of them can be attached to a color,

/// @brief storage format of the frame buffer
enum class FramebufferChannelFormat { RGB, RGBA, DEPTH };

/// @brief type of framebuffer. Texture is readable, Renderbuffer not but is more efficient.
enum class FramebufferDataType { UNSIGNED_BYTE, HALF, FLOAT32};

enum class FramebufferBufferType { UNUSED, TEXTURE, RENDERBUFFER };

//enum class FramebufferAttachment { COLOR, DEPTH, STENCIL, DEPTH_STENCIL };

struct Buffer {
  FramebufferBufferType bufferType { FramebufferBufferType::UNUSED };
  FramebufferDataType dataType { FramebufferDataType::UNSIGNED_BYTE };
  FramebufferChannelFormat channelFormat { FramebufferChannelFormat::RGBA };
  int multisamples {0};
};

class FramebufferConfiguration {
 public:
  Buffer colorAttachments[4];
  Buffer depthAttachment;
  Buffer depthStencilAttachment;
  Buffer stencilAttachment;

//  Buffer out_colorAttachments[4];
};

struct EffectNode {
  std::shared_ptr<lsw::geo::Material> material;

  FramebufferConfiguration framebuffer;
};

struct Edge {
  std::array<short, 4> binding = {0, 1, 2, 3};
  short from, to;
};

struct BufferMapping {
  // buffer mappings. buffer[0] = 1 means output buffer 0, maps to input buffer 1 in connected node.
  int buffers[4] = {0, 1, 2, 3};
};

class Effect {
 public:
  std::string name;
  Graph<16, EffectNode, std::string, BufferMapping> graph;
};

struct cEffect {
  std::string name;
//  Graph<std::shared_ptr<lsw::geo::Material>> graph;
};

}  // namespace gl
}  // namespace izz
