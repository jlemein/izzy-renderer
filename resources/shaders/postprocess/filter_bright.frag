#version 460
#extension GL_ARB_separate_shader_objects : enable

const float BRIGHTNESS_THRESHOLD = 0.7;

layout(binding = 0) uniform sampler2D scene;

layout(location = 0) in vec2 in_uv;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 color = texture(scene, in_uv);
    if (color.x < BRIGHTNESS_THRESHOLD) {
        color.x = 0.0;
    }
    if (color.y < BRIGHTNESS_THRESHOLD) {
        color.y = 0.0;
    }
    if (color.z < BRIGHTNESS_THRESHOLD) {
        color.z = 0.0;
    }

    outColor = color;
}