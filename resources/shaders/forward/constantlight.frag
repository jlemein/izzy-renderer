#version 460
#extension GL_ARB_separate_shader_objects : enable

#include "/common/depth_peeling.glsl"

#define EPSILON 0.00001

layout(binding = 1)
uniform ModelViewProjection {
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

layout(location = 0) out vec4 out_color;

void main() {
#ifdef DEPTHPEELING
    DEPTH_PEEL_DISCARD_CHECK();
#endif

    out_color = vec4(color.rgb*intensity, color.a);

#ifdef DEPTHPEELING
    DEPTH_PEEL_BLEND_OVER();
#endif
}