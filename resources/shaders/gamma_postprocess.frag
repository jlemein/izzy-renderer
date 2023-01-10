#version 460

layout (location = 0) out vec4 out_color;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;

uniform sampler2D hdrTexture; // cubemap texture sampler

layout(std140, binding = 0)
uniform GammaCorrection {
    float invGamma;
};

void main()
{
    out_color = texture(hdrTexture, in_uv);

    // GAMMA CORRECTION
    out_color.rgb = pow(out_color.rgb, vec3(invGamma));
    out_color.w = 0.0;
}