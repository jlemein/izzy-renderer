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

struct BufferedMeshData;
struct RenderState;
struct UniformBufferMapping;

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

  /**
   * Activates all uniform properties for the specified render state.
   * This includes the uniform buffer blocks, the named uniform properties
   * and eventual shader binding blocks.
   * @param [in] rs
   */
  static void PushUniformProperties(const RenderState& rs);

  /***
   * Activates all unscoped uniforms. Unscoped uniforms are not part of any
   * buffer block (such as uniform buffer block).
   * @param [in] rs Render state
   */
  static void ActivateUnscopedUniforms(const RenderState& rs);

  /**
   * Loads the shaders from the material, and stores the id's of the GPU buffers in render state.
   * This function does not synchronize the data from material in the shader (uniform) buffers.
   * @param [in] m      Material
   * @param [out] rs    Render state to update.
   */
  static void LoadMaterial(const izz::gl::Material& m, RenderState& rs);

//  static int GetUniformBufferLocation(const RenderState& rs, std::string uboName);
  static UniformBufferMapping GetUniformBufferLocation(const RenderState& rs, std::string uboName);


};

}  // namespace gl
}  // namespace izz
