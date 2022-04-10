#version 460

layout(std140, binding=0)
uniform ColorBlock {
    vec4 uColor;
};

layout(std140, binding = 1)
uniform ModelViewProjection {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPosition; // position of camera in world coordinates
};

layout(location = 0) in vec3 position;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec2 out_uv;

void main() {
    mat4 MVP = proj * view * model;

    gl_Position = MVP * vec4(position, 1.0);
}