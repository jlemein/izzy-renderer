#version 460
#extension GL_ARB_separate_shader_objects : enable

#define MAX_POINT_LIGHTS 4
#define MAX_SPOT_LIGHTS 2


struct PointLight {
    vec4 position;
    vec4 color;
    float intensity;
    float attenuation;
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
    vec3 color;
    float intensity;
};

layout(std140, binding=2)
uniform ForwardLighting {
    ivec4 numberOfLights;
    DirectionalLight directionalLight;
    AmbientLight ambientLight;
    PointLight pointLights[MAX_POINT_LIGHTS];
    SpotLight spotlight [MAX_SPOT_LIGHTS];
};

layout(std140, binding = 1)
uniform ModelViewProjection {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPosition;// position of camera in world coordinates
};

layout(binding = 2)
uniform Parallax {
    float height_scales;
};


layout(location = 0) in vec4 in_normal;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec4 in_TangentLightPosition;
layout(location = 3) in vec3 in_TangentViewPosition;
layout(location = 4) in vec3 in_TangentFragPosition;
layout(location = 5) in vec3 in_tangent;
layout(location = 6) in vec3 in_btangent;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D albedoMap;
layout(binding = 1) uniform sampler2D normalMap;
layout(binding = 2) uniform sampler2D heightMap;

vec4 computeDirectionalLight(vec3 surf_normal, vec3 light_direction) {
    float dot_normal_light = clamp(dot(light_direction, surf_normal), 0.0, 1.0);
    float attenuation = 1.0F;
    vec4 light_color = directionalLight.color * directionalLight.intensity;
    return dot_normal_light * attenuation * light_color;
}

vec2 ParallaxMapping(vec2 uv, vec3 viewDir) {
    float height = texture(heightMap, uv).r;
    float height_scale = 0.2;
    vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
    return uv - p;
}

vec2 SteepParallaxMapping(vec2 uv, vec3 viewDir) {
    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0));
//    float numLayers = 10;
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;

    float height_scale = 0.2;
    vec2 P = viewDir.xy * height_scale;
    vec2 deltaTexCoords = P / numLayers;

    vec2 currentTexCoords = uv;
    float currentDepthMapValue = texture(heightMap, uv).r;

    while (currentLayerDepth < currentDepthMapValue) {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = texture(heightMap, uv).r;
        currentLayerDepth += layerDepth;
    }

    return currentTexCoords;
}



void main() {
    vec3 viewDir = normalize(in_TangentViewPosition - in_TangentFragPosition);
//    vec2 uv = ParallaxMapping(in_uv, viewDir);
    vec2 uv = SteepParallaxMapping(in_uv, viewDir);

    if (uv.x > 1.0 || uv.y > 1.0 || uv.x < 0.0 || uv.y < 0.0)
        discard;

    vec4 material_color = texture(albedoMap, uv);
    vec3 surf_normal = normalize(texture(normalMap, uv).rgb*2.0-1.0);// surface normal
    vec3 light_direction = normalize(in_TangentLightPosition.xyz);

    outColor = computeDirectionalLight(surf_normal, light_direction) * material_color;
}