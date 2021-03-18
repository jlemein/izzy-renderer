#version 460
#extension GL_ARB_separate_shader_objects : enable

#define MAX_LIGHTS 4

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
uniform ParallaxMaterial {
    vec4 uStrength;
};

layout(binding = 0) uniform sampler2D albedo_map;
layout(binding = 1) uniform sampler2D normal_map;
layout(binding = 2) uniform sampler2D height_map;

layout(location = 0) in vec3 inFragPos;
layout(location = 1) in vec2 inTexCoords;
layout(location = 2) out vec3 inTangentLightPos[MAX_LIGHTS];
layout(location = 3) out vec3 inTangentViewPos;
layout(location = 4) out vec3 inTangentFragPos;

layout(location = 0) out vec4 outColor;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

void main() {
    // offset texture coordinates with parallax mapping
    vec3 viewDir = normalize(inTangentViewPos - inTangentFragPos);
    vec2 texCoords = ParallaxMapping(inTexCoords, viewDir);

    vec3 albedo = texture(albedo_map, inUv).xyz;
    vec3 normal = texture(normal_map, inUv).xyz;
    normal = normalize(normal * 2.0 - 1.0);

    for (int i=0; i<uNumberLights; i++) {
        vec3 nn_light_direction = normalize(light_direction[i].xyz);
        vec3 nn_normal = texture(normal_map, inUv.xy).xyz;//normalize(inNormal).xyz;
        float dot_normal_light = dot(nn_light_direction, nn_normal);
        float attenuation = 1.0F;
        vec3 light_diffuse = uColors[i].xyz;

        // if is point light
        if (light_direction[i].w != 0.0) {
            // point light
            float d = length(light_direction[i]);
            attenuation = clamp(2.0 / (1.0 + uAttenuation[i]*d*d), 0.0, 1.0);

            // 1000W is 1
            light_diffuse *= 0.001F * uIntensity[i];
        }

        vec3 result;
        vec3 albedo = texture(albedo_map, inUv).xyz;
        vec3 normal = texture(normal_map, inUv).xyz;
        vec3 height = texture(height_map, inUv).xyz;

        if (inUv.y < 0.33) {
            result = albedo;
        } else if (inUv.y < 0.67) {
            result = normal;
        } else {
            result = height;
        }


        outColor = (dot_normal_light * attenuation) * vec4(result * light_diffuse, 1.0);
    }
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    float height =  texture(height_map, texCoords).r;
    vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
    return texCoords - p;
}