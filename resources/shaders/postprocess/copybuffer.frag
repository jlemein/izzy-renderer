#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D srcBuffer;

layout(location = 0) in vec2 in_uv;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(srcBuffer, in_uv);
//    outColor = vec4(0.0, 1.0, 0.0, 0.0);
}