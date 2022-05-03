#version 460

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUv;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec2 out_uv;

void main() {
    gl_Position = vec4(aPos, 1.0);
    out_position = aPos;
    out_normal = aNormal;
    out_uv = aUv;
}