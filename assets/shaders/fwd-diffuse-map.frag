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

layout(binding = 1)
uniform UniformBufferBlock {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPosition; // position of camera in world coordinates
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


void main() {
    vec3 material_color = texture(albedoMap, in_uv).xyz;
    vec3 surf_normal = normalize(texture(normalMap, in_uv).rgb*2.0-1.0);  // surface normal
    vec3 geom_normal = normalize(in_normal).xyz;          // geometric normal

    vec3 light_direction = normalize(in_TangentLightPosition.xyz);
    float dot_normal_light = clamp(dot(light_direction, surf_normal), 0.0, 1.0);

    float attenuation = 1.0F;
    vec4 light_diffuse = directionalLight.color;

    outColor = vec4(directionalLight.direction, 0.0F);//vec4(dot_normal_light) * attenuation * vec4(material_color * light_diffuse, 1.0);
}