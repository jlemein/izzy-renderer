#version 460

#define MAX_POINT_LIGHTS 32

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUv;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec2 out_uv;

struct PointLight {
    vec4 worldPosition;
    vec4 color;
    float intensity;
    float radius;
};

layout(std140, binding = 1)
uniform ModelViewProjection {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPosition; // position of camera in world coordinates
};

layout(std140, binding=2)
uniform DeferredLighting {
    vec3 viewPos;
    int numberOfLights;
    PointLight pointLights[MAX_POINT_LIGHTS];
};

void main() {
    gl_Position = vec4(aPos, 1.0);
    out_position = aPos;
    out_normal = aNormal;
    out_uv = aUv;
}