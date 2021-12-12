#version 460
#extension GL_ARB_separate_shader_objects : enable

uniform float brightnessThreshold = 0.7;

layout(binding = 0) uniform sampler2D scene;

layout(location = 0) in vec2 in_uv;

layout(location = 0) out vec4 outColor;


void main() {
    vec4 color = texture(scene, in_uv);
    float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness <= brightnessThreshold) {
        color = vec4(0.0, 0.0, 0.0, 1.0);
    }

    outColor = color;
}