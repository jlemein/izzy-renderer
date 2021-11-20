#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1)
uniform UniformBufferBlock {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPosition; // position of camera in world coordinates
};


// Model attributes
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUv;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

layout(location = 0) out vec2 out_uv;

void main() {
    out_uv = aUv;
    mat4 MVP = proj * view * model;
    gl_Position = MVP * vec4(aPos, 1.0);
}