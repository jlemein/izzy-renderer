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
layout(location = 2) in vec4 light_direction[MAX_LIGHTS];

layout(location = 0) out vec4 outColor;


void main() {
    for (int i=0; i<uNumberLights; i++) {
        vec3 nn_light_direction = normalize(light_direction[i].xyz);
        vec3 nn_normal = normalize(inNormal).xyz;
        float dot_normal_light = dot(nn_light_direction, nn_normal);
        float attenuation = 1.0F;

        // if is point light
        if (light_direction[i].w != 0.0) {
            // point light
            float d = length(light_direction[i]);
            attenuation = 2.0 / (1.0 + d*d);
        }

        vec3 light_diffuse = (uIntensities[i] * uColors[i]).xyz;

        outColor += dot_normal_light * attenuation * vec4(uDiffuse * light_diffuse, 1.0);
    }
}