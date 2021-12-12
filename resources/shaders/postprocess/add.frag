#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D bright;
layout(binding = 1) uniform sampler2D scene;

layout(location = 0) in vec2 in_uv;

layout(location = 0) out vec4 outColor;

void main() {
    const float gamma = 2.2;
    const float exposure = 0.5;
    vec3 hdrColor = texture(scene, in_uv).rgb;
    vec3 bloomColor = texture(bright, in_uv).rgb;
    hdrColor += bloomColor; // additive blending
    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    // also gamma correct while we're at it
    result = pow(result, vec3(1.0 / gamma));
    outColor = vec4(result, 1.0);
}