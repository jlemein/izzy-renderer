#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1)
uniform UniformBufferBlock {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPosition;// position of camera in world coordinates
};

layout(location = 0) in vec4 in_normal;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = normalize(in_normal);
}