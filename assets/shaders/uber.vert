#version 460

#define MAX_LIGHTS 4

layout(std140, binding=2)
uniform Lighting {
// Positions in world space
    vec4 uPositions[MAX_LIGHTS];
    vec4 uIntensities[MAX_LIGHTS];
    vec4 uColors[MAX_LIGHTS];
    int uNumberLights;
};

layout(std140, binding=4)
uniform UberMaterial {
    vec3 uDiffuse;
    vec3 uSpecular;
    vec3 uAmbient;
};

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

layout(location = 2) out vec3 light_direction[MAX_LIGHTS];

void main() {
    mat4 MVP = proj * view * model;
    mat4 MV = view * model;
    mat4 invTranspose = inverse(transpose(model));

    vec4 world_position = model * vec4(position, 1.0F);

    gl_Position = MVP * vec4(position, 1.0);
    out_uv = uv;

    for (int i=0; i<uNumberLights; i++) {
        light_direction[i] = (uPositions[i] - world_position).xyz;
    }
    normalOut = invTranspose * vec4(normal, 1.0);
}