#version 460
#extension GL_ARB_separate_shader_objects : enable

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

layout(location = 0) in vec4 inNormal;
layout(location = 1) in vec2 uv1;
layout(location = 2) in vec3 light_direction[MAX_LIGHTS];

layout(location = 0) out vec4 outColor;


void main() {
    for (int i=0; i<uNumberLights; i++) {
        vec3 nn_light_direction = normalize(light_direction[i]);
        vec3 nn_normal = normalize(inNormal).xyz;
        float dot_normal_light = dot(nn_light_direction, nn_normal);

        vec3 material_color = uDiffuse;
        vec3 light_contribution = (uIntensities[i] * uColors[i]).xyz;

        outColor += dot_normal_light * vec4(material_color * light_contribution, 1.0);
    }
}