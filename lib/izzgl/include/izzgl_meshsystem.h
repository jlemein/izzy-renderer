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

  void bindBuffer(const MeshBuffer& meshBuffer);
  void bindBuffer(izz::VertexBufferId meshBufferId);

 private:
  std::unordered_map<VertexBufferId, MeshBuffer> m_allocatedBuffers;
};

}  // namespace gl
}  // namespace izz
