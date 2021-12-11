#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D scene;
layout(binding = 1) uniform sampler2D bright;

layout(location = 0) in vec2 in_uv;

layout(location = 0) out vec4 outColor;

uniform float weights[5];// = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
uniform bool horizontal;// = false;

void main() {
    vec2 tex_offset = 1.0 / textureSize(scene, 0);
    vec3 result = texture(scene, in_uv).rgb * weights[0];

    if (horizontal) {
        for (int i=1; i<5; ++i) {
            result += texture(scene, in_uv + vec2(tex_offset.x * i, 0.0)).rgb * weights[i];
            result += texture(scene, in_uv - vec2(tex_offset.x * i, 0.0)).rgb * weights[i];
        }
    } else {
        for (int i=1; i<5; ++i) {
            result += texture(scene, in_uv + vec2(0.0, tex_offset.x * i)).rgb * weights[i];
            result += texture(scene, in_uv - vec2(0.0, tex_offset.x * i)).rgb * weights[i];
        }
    }

    outColor = vec4(result, 1);
}