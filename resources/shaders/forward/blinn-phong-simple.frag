#version 460
#extension GL_ARB_separate_shader_objects : enable

#include "/common/lights.glsl"
#include "/common/depth_peeling.glsl"

#define MAX_DIRECTIONAL_LIGHTS 1
#define MAX_POINT_LIGHTS 4
#define MAX_SPOT_LIGHTS 2
#define EPSILON 0.00001

layout(std140, binding=2)
uniform ForwardLighting {
    int numberOfDirectionalLights;
    int numberOfAmbientLights;
    int numberOfPointLights;
    int numberOfSpotLights;
    DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];  // largest base alignment: 4. Starts at byte 16 (directly after numberOfLights)
    AmbientLight ambientLight; // largest base alignment: 4.
    PointLight pointLights[MAX_POINT_LIGHTS];
    SpotLight spotLights[MAX_SPOT_LIGHTS];
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

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in vec3 in_tangent;

layout(location = 0) out vec4 out_color;

void main() {
#ifdef DEPTHPEELING
    DEPTH_PEEL_DISCARD_CHECK();
#endif

    // create TBN matrix (tangent to world space)
    // needed to map the normal map data (in tangent space) to world space.
    vec3 T = normalize(in_tangent);
    vec3 N = normalize(in_normal);
    vec3 B = cross(T, N);
    mat3 TBN = mat3(T, B, N);

    vec3 normal = normalize(in_normal.xyz);

    vec3 albedo = material_color.rgb;
    float specularity = material_color.w;
    vec3 p = in_position;
    vec3 v = normalize(viewPosition.xyz - p.xyz);

    out_color = vec4( ambientLight.color.rgb * albedo.rgb, 0.0);

    for(int i=0; i<numberOfDirectionalLights; i++) {
        DirectionalLight light = directionalLights[i];
        out_color.rgb += GetDirectionalLightContribution_shininess(light, normal, v, albedo.rgb, specularity, shininess);
    }

    for (int i=0; i<numberOfPointLights; ++i) {
        PointLight light = pointLights[i];
        out_color.rgb += GetPointLightContribution_shininess(light, p, normal, v, albedo.rgb, specularity, shininess);
    }

    for (int i=0; i<numberOfSpotLights; i++) {
        SpotLight light = spotLights[i];
        out_color.rgb += GetSpotLightContribution_shininess(light, p, normal, v, albedo.rgb, specularity, shininess);
    }

    out_color.a = material_color.a;

#ifdef DEPTHPEELING
    DEPTH_PEEL_BLEND_OVER();
#endif
}