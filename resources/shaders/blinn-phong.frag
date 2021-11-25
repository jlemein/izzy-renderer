#version 460
#extension GL_ARB_separate_shader_objects : enable

#define MAX_POINT_LIGHTS 4
#define MAX_SPOT_LIGHTS 2

struct PointLight {
    vec4 position;
    vec4 color;
    float intensity;    // intensity
    float lAttenuation; // linear attenuation factor
    float qAttenuation; // quadratic attenuation factor
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
    vec3 viewPosition; // position of camera in world coordinates
};

layout(std140, binding = 2)
uniform BlinnPhong {
    float shininess;
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
layout(binding = 2) uniform sampler2D specularMap;
layout(binding = 2) uniform sampler2D roughnessMap;

const float MAX_SHININESS = 100.0;

float attenuation(PointLight light, float dist) {
    return 1.0 / (1.0 + light.lAttenuation * dist + light.qAttenuation * dist*dist);
}

vec4 computeDirectionalLight(vec3 surf_normal, vec3 light_direction) {
    vec3 viewDir = normalize(in_TangentViewPosition);
    vec4 light_color = directionalLight.color * directionalLight.intensity;
    float specularity = texture(specularMap, in_uv).r;

    // DIFFUSE PART
    float dot_normal_light = clamp(dot(light_direction, surf_normal), 0.0, 1.0);
    vec4 diffuse = dot_normal_light * light_color * (1.0 - specularity);

    // SPECULAR PART (BLINN-PHONG)
    vec3 halfway = normalize(light_direction + viewDir);
    float glossyness = 1.0 - texture(roughnessMap, in_uv).r;
    vec4 specular = pow(max(dot(surf_normal, halfway), 0.0), glossyness*MAX_SHININESS) * light_color * specularity;

    // PHONG PART
    //vec3 reflectDir = reflect(-lightDir, surf_normal);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

//    diffuse *= (1.0 - specularity);

    return diffuse + specular;
}

vec4 computeAmbientLight() {
    return ambientLight.color * ambientLight.intensity;
}

vec4 computePointLight(vec3 surf_normal, vec3 light_position, PointLight light) {
    vec3 viewDir = normalize(in_TangentViewPosition);
    vec4 light_color = light.color * light.intensity;
    vec3 light_direction = normalize(light_position);
    float specularity = texture(specularMap, in_uv).r;

    // DIFFUSE PART
    float dist = length(light_position);
    float attenuation = attenuation(light, dist);
    float dot_normal_light = clamp(dot(light_direction, surf_normal), 0.0, 1.0);
    vec4 diffuse = attenuation * dot_normal_light * light_color * (1.0 - specularity);

    // SPECULAR PART (BLINN-PHONG)
    vec3 halfway = normalize(light_direction + viewDir);
    float glossyness = 1.0 - texture(roughnessMap, in_uv).r;
    vec4 specular = pow(max(dot(surf_normal, halfway), 0.0), glossyness*MAX_SHININESS) * light_color * specularity;


    return diffuse + specular;
    //return dot_normal_light * attenuation * light.intensity * light.color;
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