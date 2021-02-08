#version 460
#extension GL_ARB_separate_shader_objects : enable

#define MAX_LIGHTS 4

layout(std140, binding=2)
uniform Lighting {
    // Positions in world space
    vec4 uPositions[MAX_LIGHTS];
    vec4 uColors[MAX_LIGHTS];
    float uIntensity[MAX_LIGHTS];
    float uAttenuation[MAX_LIGHTS];
    int uNumberLights;
};

layout(location = 0) in vec4 inNormal;
layout(location = 1) in vec2 uv1;
layout(location = 2) in vec4 light_direction[MAX_LIGHTS];

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D diffuseTexture;


void main() {
    for (int i=0; i<uNumberLights; i++) {
        vec3 nn_light_direction = normalize(light_direction[i].xyz);
        vec3 nn_normal = normalize(inNormal).xyz;
        float dot_normal_light = dot(nn_light_direction, nn_normal);

        vec3 material_color = texture(diffuseTexture, uv1).xyz;

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

        outColor += dot_normal_light * attenuation * vec4(material_color * light_diffuse, 1.0);
    }
}