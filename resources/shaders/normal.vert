#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1)
uniform UniformBufferBlock {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPosition; // position of camera in world coordinates
};

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

layout(location = 0) out vec4 out_normal;

void main() {
    mat4 invTranspose = inverse(transpose(model));
    out_normal = invTranspose * vec4(aNormal, 0.0);

    mat4 MVP = proj * view * model;
    gl_Position = MVP * vec4(aPos, 1.0);
}