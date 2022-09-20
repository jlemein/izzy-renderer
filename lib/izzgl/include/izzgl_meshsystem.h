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
  MeshBuffer& createMeshBuffer(const izz::geo::Mesh& mesh);

  inline MeshBuffer& getMeshBuffer(MeshBufferId id) {
    return m_allocatedBuffers.at(id);
  }

  void bindBuffer(const MeshBuffer& meshBuffer);
  void bindBuffer(izz::MeshBufferId meshBufferId);

 private:
  std::unordered_map<MeshBufferId, MeshBuffer> m_allocatedBuffers;
};

}  // namespace gl
}  // namespace izz
