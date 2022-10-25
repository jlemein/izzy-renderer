#version 460
#extension GL_ARB_separate_shader_objects : enable

#define MAX_DIRECTIONAL_LIGHTS 1
#define MAX_POINT_LIGHTS 4
#define MAX_SPOT_LIGHTS 2
#define EPSILON 0.00001
#define SHININESS 32.0

struct PointLight {
    vec4 position;
    vec4 color;
    float intensity;
    float radius;
};

struct DirectionalLight {
    vec4 direction;
    vec4 color;
};

struct SpotLight {
    vec4 position_umbra;
    vec4 direction_penumbra;
    vec4 color;
};

struct AmbientLight {
    vec4 color;     // correct at 0 - 16 bytes
};

layout(std140, binding = 1)
uniform ModelViewProjection {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPosition;// position of camera in world coordinates
};

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

#ifdef DEPTHPEELING
layout(std140, binding = 4)
uniform DepthPeeling {
    bool isPeelingPass;
    ivec2 screenSize;
};

layout(binding = 0) uniform sampler2D depthMap0; // opaque objects' depth map.
layout(binding = 1) uniform sampler2D depthMap1; // transparent objects' depth map (peeled depth).
layout(binding = 2) uniform sampler2D colorMap;  // combined transparent objects' color contribution.
#endif// DEPTHPEELING

layout(binding = 3) uniform sampler2D albedoMap;
layout(binding = 4) uniform sampler2D normalMap;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in vec3 in_tangent;

layout(location = 0) out vec4 out_color;

vec3 GetDirectionalLightContribution(DirectionalLight light, vec3 n, vec3 v, vec3 albedo, float specularity) {
    vec3 l = normalize(light.direction.xyz);

    float lambert = max(0.0, dot(n, l));
    vec3 diffuse = albedo * light.color.rgb * lambert;

    // compute specular intensity using half angle
    vec3 h = normalize(l + v);
    float ndoth = clamp(dot(n, h), 0.0, 1.0);
    float specular_intensity = specularity * pow(ndoth, SHININESS);
    vec3 specular = specular_intensity * light.color.rgb;

    return diffuse + specular;
}

vec3 GetPointLightContribution(PointLight light, vec3 p, vec3 n, vec3 v, vec3 albedo, float specularity) {
    vec3 l = light.position.xyz - p;
    float dist = length(l);
    l = normalize(l);

    float lambertian = max(0.0, dot(n, l));
    vec3 diffuse = albedo.rgb * light.color.rgb * lambertian;

    // compute specular intensity using half angle
    vec3 h = normalize(l + v);
    float ndoth = clamp(dot(n, h), 0.0, 1.0);
    float specular_intensity = specularity * pow(ndoth, SHININESS);
    vec3 specular = specular_intensity * light.color.rgb;

    // add diffuse and specular component
    return (diffuse + specular) / (dist*dist);
}

vec3 GetSpotLightContribution(SpotLight light, vec3 p, vec3 n, vec3 v, vec3 albedo, float specularity) {
    vec3 l = light.position_umbra.xyz - p;// dir to light
    float dist = length(l);
    l = normalize(l);

    float cos_s = dot(-l, normalize(light.direction_penumbra.xyz));
    float cos_u = light.position_umbra.w;
    float cos_p = light.direction_penumbra.w;
    float t = clamp((cos_s - cos_u)/(cos_p - cos_u), 0.0, 1.0);

    float lambertian = max(0.0, dot(n, l));
    vec3 diffuse = albedo * lambertian * light.color.rgb / (dist*dist);

    // compute specular intensity using half angle
    vec3 h = normalize(l + v);
    float ndoth = clamp(dot(n, h), 0.0, 1.0);
    float specular_intensity = specularity * pow(ndoth, SHININESS);
    vec3 specular = specular_intensity * light.color.rgb / (dist*dist);

    return t*t*(diffuse + specular);
}

void main() {
#ifdef DEPTHPEELING
    vec2 uv = gl_FragCoord.xy / screenSize;

    if (isPeelingPass) {
        float depth0 = texture(depthMap0, uv).r;
        float depth1 = texture(depthMap1, uv).r;

        if (gl_FragCoord.z < depth1 + EPSILON || gl_FragCoord.z > depth0) {
            discard;
        }
    }
#endif
    // create TBN matrix (tangent to world space)
    // needed to map the normal map data (in tangent space) to world space.
    vec3 T = normalize(in_tangent);
    vec3 N = normalize(in_normal);
    vec3 B = cross(T, N);
    mat3 TBN = mat3(T, B, N);

    vec3 normal = texture(normalMap, in_uv).rgb;
    normal = normalize(normal * 2.0 - 1.0); // transform normal vector to range [-1,1]
    normal = vec4(TBN * normal, 0).xyz;

    vec4 albedo = texture(albedoMap, in_uv);
    vec3 p = in_position;
    vec3 v = normalize(viewPosition.xyz - p.xyz);

    out_color = vec4(ambientLight.color.rgb * albedo.rgb, 0.0);

    for(int i=0; i<numberOfDirectionalLights; i++) {
        DirectionalLight light = directionalLights[i];
        out_color.rgb += GetDirectionalLightContribution(light, normal, v, albedo.rgb, 0.0);
    }

    for (int i=0; i<numberOfPointLights; ++i) {
        PointLight light = pointLights[i];
        out_color.rgb += GetPointLightContribution(light, p, normal, v, albedo.rgb, 0.0);
    }

    for (int i=0; i<numberOfSpotLights; i++) {
        SpotLight light = spotLights[i];
        out_color.rgb += GetSpotLightContribution(light, p, normal, v, albedo.rgb, 0.0);
    }

    out_color.a = 1.0;//material_color.a;

#ifdef DEPTHPEELING
    if (isPeelingPass) {
        vec4 color_d = texture(colorMap, uv);

        // blend under-operator, from Real-Time Rendering 4th edition (p.153)
        // I believe there is an error in the book and this one should be correct.
        outColor.rgb = /*color_d.a * */ color_d.rgb + (1.0 - color_d.a) * outColor.a * outColor.rgb;
        outColor.a = outColor.a * (1-color_d.a) + color_d.a;
    }
#endif
}