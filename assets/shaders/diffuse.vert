#version 460

//layout(binding = 0)
//buffer UniformBufferBlock {
//    mat4 model;
//    mat4 view;
//    mat4 proj;
//} ubo;

layout(binding = 1)
uniform UniformBufferBlock {
    mat4 model;
    mat4 view;
    mat4 proj;
};

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

layout(location = 0) out vec4 normalOut;
layout(location = 1) out vec2 out_uv;

void main() {
    mat4 MVP = proj * view * model;
    mat4 MV = view * model;
    mat4 transposedMV = transpose(MV);

    normalOut = transposedMV * vec4(normal, 1.0);
    gl_Position = MVP * vec4(position, 1.0);
    out_uv = uv;
}