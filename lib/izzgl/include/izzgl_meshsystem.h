//
// Created by jeffrey on 27-4-22.
//
#pragma once

#include <izz.h>
#include <izzgl_meshbuffer.h>

namespace izz {
namespace geo {
struct Mesh;
}
}  // namespace lsw

namespace izz {
namespace gl {

class MeshSystem {
 public:
  MeshBuffer& createVertexBuffer(const izz::geo::Mesh& mesh);

  inline MeshBuffer& getMeshBuffer(VertexBufferId id) {
    return m_allocatedBuffers.at(id);
  }

  /**
   *
   * @param meshBuffer
   */
  void bindBuffer(const MeshBuffer& meshBuffer);
  void bindBuffer(izz::VertexBufferId meshBufferId);

  /**
   * Only bind the vertex buffer.
   * @param vbo
   */
  void bindVertexBuffer(izz::VertexBufferId vbo);

  /**
   * Only bind the index buffer
   * @param meshBuffer
   */
  void bindIndexBuffer(const MeshBuffer& meshBuffer);

 private:
  std::unordered_map<VertexBufferId, MeshBuffer> m_allocatedBuffers;
};

}  // namespace gl
}  // namespace izz
