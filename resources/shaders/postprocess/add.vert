#version 460
#extension GL_ARB_separate_shader_objects : enable

// Model attributes
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUv;

layout(location = 0) out vec2 out_uv;

void main() {
    out_uv = aUv;
    gl_Position = vec4(aPos, 0.0, 1.0);
}