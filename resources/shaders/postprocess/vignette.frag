#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D srcBuffer;

layout(location = 0) in vec2 in_uv;

layout(location = 0) out vec4 outColor;

void main() {
    vec2 delta = abs(in_uv - vec2(0.5, 0.5));
    if (length(delta) > 0.3) {
        outColor = vec4(0);
    } else {
        outColor = texture(srcBuffer, in_uv);
    }
}