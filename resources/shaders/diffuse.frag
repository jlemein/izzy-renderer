#version 460
#extension GL_ARB_separate_shader_objects : enable

#define MAX_POINT_LIGHTS 4
#define MAX_SPOT_LIGHTS 2

struct PointLight {
    vec4 position;
    vec4 color;
    float intensity;
    float lAttenuation;
    float qAttenuation;
};

struct DirectionalLight {
    vec3 direction;
    float intensity;
    vec4 color;
};

struct SpotLight {
    vec4 position;
};

struct AmbientLight {
    vec4 color;     // correct at 0 - 16 bytes
    float intensity; // starting at 16 bytes
};

layout(std140, binding=2)
uniform ForwardLighting {
    ivec4 numberOfLights;  // dir, ambient, pt, sptlgt
    DirectionalLight directionalLight;  // largest base alignment: 4. Starts at byte 16 (directly after numberOfLights)
    AmbientLight ambientLight; // largest base alignment: 4.
    PointLight pointLights[MAX_POINT_LIGHTS];
    SpotLight spotlight [MAX_SPOT_LIGHTS];
};

layout(binding = 1)
uniform UniformBufferBlock {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPosition;// position of camera in world coordinates
};


layout(location = 0) in vec4 in_normal;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec4 in_TangentLightPosition;
layout(location = 3) in vec3 in_TangentViewPosition;
layout(location = 4) in vec3 in_TangentFragPosition;
layout(location = 5) in vec3 in_tangent;
layout(location = 6) in vec3 in_btangent;
layout(location = 7) in vec4 in_TangentPtLightPosition[MAX_POINT_LIGHTS];

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D albedoMap;
layout(binding = 1) uniform sampler2D normalMap;

float attenuation(PointLight light, float dist) {
    return 1.0 / (1.0 + light.lAttenuation * dist + light.qAttenuation * dist*dist);
}

vec4 computeDirectionalLight(vec3 surf_normal, vec3 light_direction) {
    float dot_normal_light = clamp(dot(light_direction, surf_normal), 0.0, 1.0);
    vec4 light_color = directionalLight.color * directionalLight.intensity;
    return dot_normal_light * light_color;
}

vec4 computeAmbientLight() {
    return ambientLight.color * ambientLight.intensity;
}

vec4 computePointLight(vec3 surf_normal, vec3 light_position, PointLight light) {
    float dist = length(light_position);
    vec3 light_direction = normalize(light_position);

    float dot_normal_light = clamp(dot(light_direction, surf_normal), 0.0, 1.0);
    float attenuation = attenuation(light, dist);

    return dot_normal_light * attenuation * light.intensity * light.color;
}

void main() {
    vec4 material_color = texture(albedoMap, in_uv);
    vec3 surf_normal = normalize(texture(normalMap, in_uv).rgb*2.0-1.0);// surface normal
    vec3 geom_normal = normalize(in_normal.xyz);// geometric normal
    vec3 light_direction = normalize(in_TangentLightPosition.xyz);

    if (numberOfLights.x > 0) {
        outColor += computeDirectionalLight(surf_normal, light_direction) * material_color;
    }
    if (numberOfLights.y > 0) {
        outColor += computeAmbientLight() * material_color;
    }
    for (int i=0; i<numberOfLights.z; ++i) {
        vec3 light_vector = in_TangentPtLightPosition[i].xyz - in_TangentFragPosition;
        outColor += computePointLight(surf_normal, light_vector, pointLights[i]) * material_color;
    }
}