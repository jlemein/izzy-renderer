#version 460

layout(location = 0) in vec3 position;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec2 out_uv;

void main() {
    gl_Position = vec4(position, 1.0);
}