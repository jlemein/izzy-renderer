//
// Created by jeffrey on 07-11-21.
//
#pragma once

#include <GL/glew.h>
#include "izzgl_material.h"

namespace izz {
namespace gl {

using CompileConstants = izz::geo::Capabilities;

struct ShaderContext {
  std::unordered_map<std::string, std::vector<std::string>> m_includes;
};

class ShaderCompiler {
public:
    using Program = GLint;

    ShaderCompiler();

    /**
     * Compiles a GLSL vertex and fragment shader from source.
     * @param vertexShader Path to the source file containing GLSL code for the vertex shader
     * @param fragmentShader Path to the source file containing GLSL code for the fragment shader.
     * @return A compiled, linked program object.
     * @throws runtime_error when compilation of linking of shader objects fail.
     */
    Program compileShader(const std::string& vertexShader, const std::string& fragmentShader);

    /**
     * Links a pre-compiled SPIRV vertex and fragment shader.
     * @param vertexShader Path to the binary SPIRV file for the vertex shader.
     * @param fragmentShader Path to the binary SPIRV file for the fragment shader.
     * @return A linked shader program object.
     * @throws runtime_error when linking of shader objects fail.
     */
    Program compileSpirvShader(const std::string& vertexShader, const std::string& fragmentShader);

    /**
     * @returns true if GPU/system supports binary or precompiled shaders. If not, shaders need to be compiled from source before used.
     */
    bool hasBinaryShaderSupport();

    void setCompileConstant(std::string key, std::string value);
    void enableCompileConstant(std::string key);

private:
    bool m_hasBinaryShaderSupport {false};

    CompileConstants m_compileConstants;

    void addCompileConstants(std::vector<std::string>& lines);


};

} // ecs
} // lsw
