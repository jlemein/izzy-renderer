#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1)
uniform UniformBufferBlock {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPosition;// position of camera in world coordinates
};

layout(binding = 3)
uniform ConstantLight {
    vec4 color;
    float intensity;
    float radius;
};

layout(location = 0) in vec4 in_normal;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_tangent;
layout(location = 3) in vec3 in_btangent;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = color*intensity;
}