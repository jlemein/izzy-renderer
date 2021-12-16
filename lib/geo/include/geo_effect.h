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

enum class FramebufferFormat { UNUSED, RGBA_FLOAT32, RGBA_UINT8 };
class FramebufferConfiguration {
 public:
  FramebufferFormat colorBuffers[4] = {FramebufferFormat::UNUSED, FramebufferFormat::UNUSED, FramebufferFormat::UNUSED, FramebufferFormat::UNUSED};
  FramebufferFormat depthBuffer;
  FramebufferFormat outColorBuffers[4] = {FramebufferFormat::UNUSED, FramebufferFormat::UNUSED, FramebufferFormat::UNUSED, FramebufferFormat::UNUSED};

  int getInputSlotCount() {
    int count = 0;
    for (int i=0; i<4; ++i) {
      if (colorBuffers[i] != FramebufferFormat::UNUSED) {
        count++;
      }
    }
    return count;
  }
};

//template <typename T>
struct Node {
  std::shared_ptr<lsw::geo::Material> material;

  FramebufferConfiguration inputs;
  FramebufferConfiguration outputs;
};

struct Edge {
  short from, to;
};

template <typename T, int n=128, int e=128>
class Graph {
 public:
  Graph() {
    nodes.reserve(1024);
    edges.reserve(1024);
  }

  short addNode(Node node) {
    nodes.push_back(node);
    return nodes.size()-1;
  }

  short addEdge(Edge edge) {
    edges.push_back(edge);
    return edges.size()-1;
  }

  T& getData() {
    return data;
  }

  std::vector<Node> nodes;
  std::vector<Edge> edges;
  T data;
};

using Effect = Graph<lsw::geo::Material*>;

}  // namespace gl
}  // namespace izz
