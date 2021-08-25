#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 inNormal;
layout(location = 0) out vec4 outColor;

void main() {
    vec3 absNormal = abs(inNormal.xyz);
    outColor = vec4(absNormal, 1.0);
}