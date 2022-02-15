//
// Created by jeffrey on 16-12-21.
//
#pragma once

#include <geo_material.h>
#include <geo_graph.h>
#include <memory>
#include <gl_renderable.h>

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

//  lsw::glrs::Renderable renderPass;
};

//struct Edge {
//  std::array<short, 4> binding = {0, 1, 2, 3};
//  short from, to;
//};

/**
 * Represents the mapping between the output of a pass, to the input of the next pass.
 */
struct BufferMap {
  /// The output location of the framebuffer, i.e. in shaders this is the ?? in "layout(location=???) out_color;"
  int outLocation;

  /// The texture buffer id (generated from glGenTextures) to which the texture is rendered.
  int textureBuffer;

  /// The texture unit (corresponding to the texture unit in the shader) to map to.
  int inTextureUnit;
};
/**
 * A BufferMapping is used to map output of a single render pass (or shader/program run)
 * to a subsequent render pass texture unit.
 * For example: the output of a noise pass, might result in layout(0) to produce output needed
 * in texture unit 4 in the following render pass.
 */
struct BufferMapping {
  std::vector<BufferMap> bufferMappings;
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
