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
layout(location = 2) in vec2 aUv;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

layout(location = 0) out vec4 out_normal;
layout(location = 1) out vec2 out_uv;
layout(location = 5) out vec3 out_tangent;
layout(location = 6) out vec3 out_btangent;

void main() {
    out_tangent = aTangent;
    out_btangent = aBitangent;
    out_uv = aUv;

    mat4 invTranspose = inverse(transpose(model));
    out_normal = invTranspose * vec4(aNormal, 1.0);

    mat4 MVP = proj * view * model;
    mat4 MV = view * model;

    gl_Position = MVP * vec4(aPos, 1.0);
}