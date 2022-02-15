//
// Created by jeffrey on 14-02-22.
//
#pragma once

#include <gl_renderable.h>

namespace lsw {
namespace geo {
struct Curve;
struct Mesh;
}  // namespace geo
}  // namespace lsw

namespace izz {
namespace gl {

struct RenderUtils {
 public:
  /**
   * Generates and fills buffers on the GPU to contain the curve data.
   * @param [in]  curve             Curve data to be moved to GPU.
   * @param [out] bufferedMeshData  Mesh data buffers that will be filled.
   */
  static void FillBufferedMeshData(const lsw::geo::Curve& curve, BufferedMeshData& bufferedMeshData);

  /**
   * @overload
   * Generates and fills buffers on the GPU to contain the mesh data.
   * @param [in]  mesh              Mesh data to be moved to GPU.
   * @param [out] bufferedMeshData  Mesh data buffers that will be filled.
   */
  static void FillBufferedMeshData(const lsw::geo::Mesh& mesh, BufferedMeshData& md);

  /**
   * @param rs
   */
//  static void ActivateProgram(const RenderState& rs);

  /**
   * Activates all textures for the specified render state.
   * After this call all textures are bound to their appropriate shader texture binds.
   * @param [in] rs                 Render state.
   */
  static void ActivateTextures(const RenderState& rs);

  /**
   * Activates the vertex and index buffer for the mesh data.
   * @param [in] md     Buffered mesh data that is already loaded on GPU.
   */
  static void UseBufferedMeshData(const BufferedMeshData& md);

};

}  // namespace gl
}  // namespace izz
