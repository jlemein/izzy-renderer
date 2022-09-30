#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(std140, binding = 1)
uniform ModelViewProjection {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPosition;// position of camera in world coordinates
};

layout(location = 0) in vec2 in_uv;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D textureMap;

void main() {
    outColor = texture(textureMap, in_uv);
}