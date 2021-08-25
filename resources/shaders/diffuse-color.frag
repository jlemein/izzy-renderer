#version 460
#extension GL_ARB_separate_shader_objects : enable

#define MAX_LIGHTS 4

layout(std140, binding=0)
uniform ColorBlock {
    vec4 uColor;
};

layout(std140, binding=1)
uniform Lighting {
    // Positions in world space
    vec4 positions[MAX_LIGHTS];
    vec4 colors[MAX_LIGHTS];
    float uInsities[MAX_LIGHTS];
    float uAttenuation[MAX_LIGHTS];
    int numberLights;
};

layout(location = 0) in vec4 inNormal;
layout(location = 0) out vec4 outColor;

void main() {
    const vec3 light_position = vec3(0.0, 2.5, 0.3);

    vec3 nn_lightdir = normalize(light_position);
    vec3 nn_normal = normalize(inNormal).xyz;
    float dot_normal_light = dot(nn_lightdir, nn_normal);

    outColor = uColor;
}