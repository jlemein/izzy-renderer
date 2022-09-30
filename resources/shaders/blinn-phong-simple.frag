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

layout(std140, binding = 1)
uniform ModelViewProjection {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPosition; // position of camera in world coordinates
};

layout(std140, binding = 3)
uniform BlinnPhongSimple {
    vec4 material_color;
    vec4 specular_color;
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

// Computes attenuation for a pointlight at a certain distance using
// a nonphysics based attenuation formula.
float attenuation(PointLight light, float dist) {
    return 1.0 / (1.0 + light.lAttenuation * dist + light.qAttenuation * dist*dist);
}

vec4 computeDirectionalLight(vec3 surf_normal, vec3 light_direction, vec3 view_direction) {
    vec4 light_color = directionalLight.color * directionalLight.intensity;

    // DIFFUSE PART
    float dot_normal_light = clamp(dot(light_direction, surf_normal), 0.0, 1.0);
    vec4 diffuse = dot_normal_light * material_color;

    // SPECULAR PART (BLINN-PHONG)
    vec3 halfway = normalize(light_direction + view_direction);
    vec4 specular = pow(max(dot(surf_normal, halfway), 0.0), shininess) * specular_color;

    return (diffuse + specular) * light_color;
}

vec4 computeAmbientLight() {
    return ambientLight.color * ambientLight.intensity;
}

vec4 computePointLight(vec3 surf_normal, vec3 light_position, vec3 view_direction, PointLight light) {
    vec4 light_color = light.color * light.intensity;
    vec3 light_direction = normalize(light_position);

    // DIFFUSE PART
    float dist = length(light_position);
    float attenuation = attenuation(light, dist);
    float dot_normal_light = clamp(dot(light_direction, surf_normal), 0.0, 1.0);
    vec4 diffuse = attenuation * dot_normal_light * material_color;

    // SPECULAR PART (BLINN-PHONG)
    vec3 halfway = normalize(light_direction + view_direction);
    vec4 specular = attenuation * pow(max(dot(surf_normal, halfway), 0.0), 2*shininess) * specular_color;

    return (diffuse + specular) * light_color;
}

void main() {
    vec3 geom_normal = normalize(in_normal.xyz);// geometric normal
    vec3 view_direction = normalize(in_TangentViewPosition - in_TangentFragPosition);

    outColor = vec4(0,0,0,0);
    if (numberOfLights.x > 0) {
        vec3 light_direction = normalize(in_TangentLightPosition.xyz);
        outColor += computeDirectionalLight(geom_normal, light_direction, view_direction);
    }
    if (numberOfLights.y > 0) {
        outColor += computeAmbientLight() * material_color;
    }
    for (int i=0; i<numberOfLights.z; ++i) {
        vec3 light_vector = in_TangentPtLightPosition[i].xyz - in_TangentFragPosition;
        outColor += computePointLight(geom_normal, light_vector, view_direction, pointLights[i]);
    }
}