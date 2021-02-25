#version 460
#extension GL_ARB_separate_shader_objects : enable

#define MAX_LIGHTS 4

//struct Light {
//    vec4 position;
//    vec4 intensity;
//    vec
//}

layout(std140, binding=2)
uniform Lighting {
    // Positions in world space
    vec4 uPositions[MAX_LIGHTS];        // 4 * 4 bytes
    vec4 uColors[MAX_LIGHTS];           // 4 * 4 bytes
    float uIntensity[MAX_LIGHTS];       // 1 * 4 bytes, 3 * 4 bytes padding
    float uAttenuation[MAX_LIGHTS];     // 1 * 4 bytes,
    int uNumberLights;
};

layout(std140, binding=4)
uniform UberMaterial {
    vec3 uDiffuse;
    vec3 uSpecular;
    vec3 uAmbient;
    bool hasDiffuseTex;
};

layout(binding = 0) uniform sampler2D diffuseTex;

layout(location = 0) in vec4 inNormal;
layout(location = 1) in vec2 inUv;
layout(location = 2) in vec4 light_direction[MAX_LIGHTS];

layout(location = 0) out vec4 outColor;

void main() {
    for (int i=0; i<uNumberLights; i++) {
        vec3 nn_light_direction = normalize(light_direction[i].xyz);
        vec3 nn_normal = normalize(inNormal).xyz;
        float dot_normal_light = dot(nn_light_direction, nn_normal);
        float attenuation = 1.0F;
        vec3 light_diffuse = uColors[i].xyz;

        // if is point light
        if (light_direction[i].w != 0.0) {
            // point light
            float d = length(light_direction[i]);
            attenuation = 2.0 / (1.0 + uAttenuation[i]*d*d);

            // 1000W is 1
            light_diffuse *= 0.001F * uIntensity[i];
        }

        vec3 diffuse = uDiffuse;
        //if (hasDiffuseTex) {
            diffuse = texture(diffuseTex, inUv).xyz;
        //}
        outColor += (dot_normal_light * attenuation + vec4(uAmbient, 1.0)) * vec4(diffuse * light_diffuse, 1.0);
    }

//    outColor = vec4(inUv, 0.5, 0.0);
}