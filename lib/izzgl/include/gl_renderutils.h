//
// Created by jeffrey on 14-02-22.
//
#pragma once

#include <gl_renderable.h>

namespace izz {
namespace geo {
struct Curve;
struct Mesh;
}  // namespace geo
}  // namespace lsw

namespace izz {
namespace gl {

struct MeshBuffer;
struct UniformBuffer;
struct Material;

struct RenderUtils {
 public:
  /**
   * Generates and fills buffers on the GPU to contain the curve data.
   * @param [in]  curve             Curve data to be moved to GPU.
   * @param [out] bufferedMeshData  Mesh data buffers that will be filled.
   */
  static void FillBufferedMeshData(const izz::geo::Curve& curve, MeshBuffer& bufferedMeshData);
};

}  // namespace gl
}  // namespace izz
