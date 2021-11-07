//
// Created by jeffrey on 07-11-21.
//
#pragma once

#include <GL/glew.h>
#include <geo_material.h>

namespace lsw {
namespace glrs {

class ShaderSystem {
public:
    using Program = GLint;

    ShaderSystem();

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

//    void readSource();
//
//    void readSprvShader();

    bool hasBinaryShaderSupport();

private:
    bool m_hasBinaryShaderSupport;
};

} // ecs
} // lsw
