#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D bright;
layout(binding = 0) uniform sampler2D scene;

layout(location = 0) in vec2 in_uv;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 RGBA = texture(srcBuffer, in_uv);
    float grayscale = dot(RGBA.xyz, vec3(0.299, 0.587, 0.114));
    outColor = vec4(vec3(grayscale), 1);
}