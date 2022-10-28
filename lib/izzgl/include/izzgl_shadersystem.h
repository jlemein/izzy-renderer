//
// Created by jeffrey on 07-11-21.
//
#pragma once

#include <GL/glew.h>
#include "izzgl_material.h"
#include "izzgl_shadersource.h"

namespace izz {
namespace gl {

class ShaderSource;

using ShaderProgram = GLint;

/**
 * Shader compiler compiles GLSL shader programs.
 */
class ShaderCompiler {
 public:
  ShaderCompiler();

  /**
   * Compiles a GLSL vertex and fragment shader from source.
   * @param vertexShader      Path to the source file containing GLSL code for the vertex shader
   * @param fragmentShader    Path to the source file containing GLSL code for the fragment shader.
   * @param pContext          Shader context used to set compile flags and additional compile-time functionality.
   *
   * @return A compiled, linked program object.
   * @throws runtime_error when compilation of linking of shader objects fail.
   */
  ShaderProgram compileShader(std::filesystem::path vertexShader, std::filesystem::path fragmentShader, ShaderContext* pContext = nullptr);

  /**
   * Links a pre-compiled SPIRV vertex and fragment shader.
   * @param vertexShader    Path to the binary SPIRV file for the vertex shader.
   * @param fragmentShader  Path to the binary SPIRV file for the fragment shader.
   *
   * @return A linked shader program object.
   * @throws runtime_error when linking of shader objects fail.
   */
  ShaderProgram compileSpirvShader(std::filesystem::path vertexShader, std::filesystem::path fragmentShader);

  /**
   * @returns true if GPU/system supports binary or precompiled shaders. If not, shaders need to be compiled from source before used.
   */
  bool hasBinaryShaderSupport();

 private:
  bool m_hasBinaryShaderSupport{false};
};

}  // namespace gl
}  // namespace izz
