#version 460

layout (location = 0) out vec4 out_color;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;

layout(binding = 0) uniform sampler2D gbuffer_position;
layout(binding = 1) uniform sampler2D gbuffer_normal;
layout(binding = 2) uniform sampler2D gbuffer_albedospec;

struct PointLight {
    vec4 position;
    vec4 color;
    float intensity;
    float radius;
};

#define MAX_POINT_LIGHTS 32

layout(std140, binding=2)
uniform DeferredLighting {
    vec3 viewPos;
    int numberOfLights;
    PointLight pointLights[MAX_POINT_LIGHTS];
};


void main()
{
    vec3 fragPos = texture(gbuffer_position, in_uv).rgb;
    vec3 normal = texture(gbuffer_normal, in_uv).rgb;
    vec3 albedo = texture(gbuffer_albedospec, in_uv).rgb;
    float specular = texture(gbuffer_albedospec, in_uv).a;

    vec3 lighting = albedo * 0.1;

    for (int i=0; i<numberOfLights; ++i) {
        PointLight pt = pointLights[i];
        vec3 lightdir = normalize(pt.position.xyz - fragPos);
        vec3 diffuse = max(0.0, dot(normal, lightdir)) * albedo * pt.color.rgb * pt.intensity;

        lighting += diffuse;
    }

    out_color = vec4(lighting, 0.0);
}