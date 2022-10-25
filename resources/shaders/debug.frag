#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(std140, binding = 1)
uniform DebugProperties {
    vec4 color;
};

layout(location = 0) out vec4 outColor;

void main() {
    outColor = color;
}