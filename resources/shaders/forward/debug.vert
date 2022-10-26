#version 460

layout(std140, binding = 1)
uniform DebugProperties {
    vec4 color;
};

layout(std140, binding = 2)
uniform ModelViewProjection {
    mat4 model;
    mat4 view;
    mat4 proj;
};

layout(location = 0) in vec3 position;

void main() {
    mat4 MVP = proj * view * model;
    gl_Position = MVP * vec4(position, 1.0);
}