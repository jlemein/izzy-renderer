#version 460
#extension GL_ARB_separate_shader_objects : enable

#define MAX_LIGHTS 4
#define MAX_POINT_LIGHTS 4
#define MAX_SPOT_LIGHTS 2


struct PointLight {
    vec4 position;
    vec4 color;
    float intensity;
    float uAttenuation[MAX_LIGHTS];
};

struct DirectionalLight {
    vec4 direction;
    vec3 color;
    float intensity;
};

struct Spotlight {
    vec4 position;
};

struct AmbientLight {
    vec3 color;
    float intensity;
};

layout(std140, binding=2)
uniform Lighting {
    ivec4 numberOfLights;
    PointLight pointLights[MAX_POINT_LIGHTS];
    SpotLight spotlight [MAX_SPOT_LIGHTS];
    DirectionalLight directionalLight;
    AmbientLight ambientLight;
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

    //  for (int i=0; i<uNumberLights; i++) {
    vec3 light_direction = normalize(in_TangentLightPosition.xyz);
    float dot_normal_light = clamp(dot(light_direction, surf_normal), 0.0, 1.0);

    float attenuation = 1.0F;
    vec3 light_diffuse = uColors[0].xyz;

    // if is point light
    if (in_TangentLightPosition.w != 0.0) {
        float d = length(light_direction);
        attenuation = 2.0 / (1.0 + uAttenuation[0]*d*d);

        // 1000W is 1
        light_diffuse *= 0.001F * uIntensity[0];
    }

    //        outColor = vec4(normalize(vec3(abs(in_tangent))), 1.0F);
    outColor = vec4(dot_normal_light) * attenuation * vec4(material_color * light_diffuse, 1.0);
    //        outColor = vec4(dot_normal_light);
    // }

    //    vec3 nn_light_direction = normalize(light_direction[0].xyz);
    //    vec3 nn_gnormal = normalize(inNormal).xyz;
    //    vec3 nn_snormal = normalize(vec3(2*texture(normalMap, uv1)-1));  // surface normal
    //    nn_snormal = normalize(TBN * nn_snormal);
    //
    //    float dot_normal_light = dot(nn_light_direction, -nn_snormal);
    //    outColor = vec4(nn_snormal, 1.0F);
}