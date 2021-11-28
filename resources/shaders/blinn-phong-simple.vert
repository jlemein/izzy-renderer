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
    vec3 direction; // 0 - 4
    float intensity; // 4
    vec4 color;
};

struct SpotLight {
    vec4 position; // 4N
};

struct AmbientLight {
    vec4 color;
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

// Model attributes
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUv;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

layout(location = 0) out vec4 out_normal;
layout(location = 1) out vec2 out_uv;
layout(location = 2) out vec4 out_TangentLightPosition; // ray towards light
layout(location = 3) out vec3 out_TangentViewPosition; // ray towards camera
layout(location = 4) out vec3 out_TangentFragPosition; // world position of shaded point
layout(location = 5) out vec3 out_tangent;
layout(location = 6) out vec3 out_btangent;
layout(location = 7) out vec4 out_TangentPtLightPosition[MAX_POINT_LIGHTS];

void main() {
    out_tangent = aTangent;
    out_btangent = aBitangent;

    vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
    vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
    mat3 TBN = transpose(mat3(T, B, N));

    out_TangentFragPosition = TBN * vec3(model * vec4(aPos, 1.0F));
    out_TangentViewPosition = TBN * viewPosition;
    out_uv = aUv;
    mat4 invTranspose = inverse(transpose(model));
    out_normal = invTranspose * vec4(aNormal, 1.0);
    out_normal = vec4(TBN * out_normal.xyz, 0.0F);

    // directional light source
    out_TangentLightPosition = vec4(TBN * directionalLight.direction.xyz, 0.0F);

    for (int i=0; i<numberOfLights.z; ++i) {
        out_TangentPtLightPosition[i] = vec4(TBN * pointLights[i].position.xyz, 0.0F);
    }

    mat4 MVP = proj * view * model;
    mat4 MV = view * model;

    gl_Position = MVP * vec4(aPos, 1.0);
}