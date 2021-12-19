//
// Created by jeffrey on 16-12-21.
//
#pragma once

#include <geo_material.h>
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
  Buffer in_colorAttachments[4];
  Buffer in_depthAttachment;
  Buffer in_depthStencilAttachment;
  Buffer in_stencilAttachment;

  Buffer out_colorAttachments[4];
};

template <typename T>
struct Node {
  T material;

  FramebufferConfiguration framebuffer;
};

struct Edge {
  std::array<short, 4> binding = {0, 1, 2, 3};
  short from, to;
};

template <typename T, int n=128, int e=128>
class Graph {
 public:

  Graph() {
    nodes.reserve(1024);
    edges.reserve(1024);
  }

  Node<T>& operator[](std::string key) {
    if (nodeIds.contains(key)) {
      return nodes[nodeIds.at(key)];
    } else {
      auto id = addNode(key);
      return nodes[id];
    }
  }

  short addNode(const std::string& key, Node<T> node = Node<T>()) {
    nodes.push_back(node);
    nodeIds[key] = nodes.size()-1;
    return nodes.size()-1;
  }

  short addEdge(Edge edge) {
    edges.push_back(edge);
    return edges.size()-1;
  }

  std::unordered_map<std::string, int> nodeIds;
  std::vector<Node<T>> nodes;
  std::vector<Edge> edges;
};

struct Effect {
  std::string name;
  Graph<std::shared_ptr<lsw::geo::Material>> graph;
};

}  // namespace gl
}  // namespace izz
