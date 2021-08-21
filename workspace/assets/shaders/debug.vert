#version 460

//layout(binding = 0)
//buffer UniformBufferBlock {
//    mat4 model;
//    mat4 view;
//    mat4 proj;
//} ubo;

layout(std140, binding = 0)
uniform ColorBlock {
    vec4 uColor;
    bool uDiffuseShading;
};

layout(std140, binding = 2)
uniform UniformBufferBlock {
    mat4 model;
    mat4 view;
    mat4 proj;
};

layout(location = 0) in vec3 position;

void main() {
    mat4 MVP = proj * view * model;

    gl_Position = MVP * vec4(position, 1.0);
}