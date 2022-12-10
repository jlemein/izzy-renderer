#version 460

#define MAX_DIRECTIONAL_LIGHTS 2
#define MAX_POINT_LIGHTS 32
#define MAX_SPOT_LIGHTS 4
#define SHININESS 32.0
//#define DEBUG // define to show quad view (contents of gbuffer).

layout (location = 0) out vec4 out_color;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;

layout(binding = 0) uniform sampler2D gbuffer_position;
layout(binding = 1) uniform sampler2D gbuffer_normal;
layout(binding = 2) uniform sampler2D gbuffer_albedospec;
layout(binding = 3) uniform sampler2D environmentMap;

struct PointLight {
    vec4 worldPosition;
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

layout(std140, binding = 1)
uniform ModelViewProjection {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPosition;// position of camera in world coordinates
};

layout(std140, binding=2)
uniform DeferredLighting {
    vec3 viewPos;
    int numberOfDirectionalLights;
    int numberOfPointLights;
    int numberOfSpotLights;
    vec4 ambient;

    DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
    PointLight pointLights[MAX_POINT_LIGHTS];
    SpotLight spotLights[MAX_SPOT_LIGHTS];
};

vec4 debugview() {
    vec4 out_color = vec4(0);
    // splitter
    if ((in_uv.x > 0.4995 && in_uv.x < 0.5005) || (in_uv.y >0.4995 && in_uv.y <= 0.5005)) {
        return vec4(0.125);
    }
    if (in_uv.x < 0.5 && in_uv.y >= 0.5) {
        out_color = texture(gbuffer_position, in_uv*2.0);
    } else if (in_uv.x >= 0.5 && in_uv.y >= 0.5) {
        out_color = texture(gbuffer_normal, (in_uv-0.5)*2.0);
    } else if (in_uv.x < 0.5 && in_uv.y < 0.5) {
        // LEFT BOTTOM
        vec2 uv = vec2(in_uv.x*2.0, in_uv.y*2.0);
        out_color = vec4(texture(gbuffer_albedospec, uv).rgb, 1);
    } else {
        // RIGHT_BOTTOM
        vec2 uv = vec2((in_uv.x-0.5)*2.0, in_uv.y*2.0);
        float spec = texture(gbuffer_albedospec, uv).w;
        out_color = vec4(spec, spec, spec, 1);
    }

    return out_color;
}

vec3 GetDirectionalLightContribution(DirectionalLight light, vec3 n, vec3 v, vec3 albedo, float specularity) {
    vec3 l = light.direction.xyz;

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
    vec3 l = light.worldPosition.xyz - p;
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
    vec3 diffuse = albedo * lambertian * light.color.rgb;

    // compute specular intensity using half angle
    vec3 h = normalize(l + v);
    float ndoth = clamp(dot(n, h), 0.0, 1.0);
    float specular_intensity = specularity * pow(ndoth, SHININESS);
    vec3 specular = specular_intensity * light.color.rgb;

    return t*t*(diffuse + specular) / (dist*dist);
}

vec3 GetEnvironmentMapContribution(vec3 v, vec3 n) {
    #define TWO_PI 6.28318530718
    #define PI 3.1415926538
    #define PI2 1.57079632679
    vec3 r = reflect(-v, n);

    vec3 d = normalize(r);
    float phi = d.x == 0.0 ? PI : atan(d.z, d.x);
    float theta = asin(d.y);
    vec2 uv = vec2(phi / TWO_PI, (theta + PI2)/PI);

    // add diffuse and specular component
    return texture(environmentMap, uv).rgb;
}

void main()
{
//    #define DEBUG
    #ifdef DEBUG
    out_color = debugview();
    #else// DEBUG

    // fetch normal and tangent from gBuffer.
    vec4 pos = texture(gbuffer_position, in_uv);
    vec3 p = pos.xyz;
    float ambient_weight = pos.w;
    vec3 n = texture(gbuffer_normal, in_uv).rgb;
    vec4 albedo_spec = texture(gbuffer_albedospec, in_uv);
    vec3 albedo = albedo_spec.rgb;
    float specularity = albedo_spec.w;

    vec3 view_position = viewPosition;
    vec3 v = normalize(view_position.xyz - p.xyz);

    if (ambient_weight == 0)
        discard;

    out_color = vec4(ambient_weight * ambient.rgb * albedo, ambient_weight);
    for (int i=0; i<numberOfDirectionalLights; i++) {
        DirectionalLight light = directionalLights[i];
        out_color.rgb += GetDirectionalLightContribution(light, n, v, albedo, specularity);
    }

    for (int i=0; i<numberOfPointLights; i++) {
        PointLight light = pointLights[i];
        out_color.rgb += GetPointLightContribution(light, p, n, v, albedo, specularity);
    }

    for (int i=0; i<numberOfSpotLights; i++) {
        SpotLight light = spotLights[i];
        out_color.rgb += GetSpotLightContribution(light, p, n, v, albedo, specularity);
    }

    // contribute HDR environment map
    out_color.rgb = out_color.rgb * 0.01 + GetEnvironmentMapContribution(v, n);

    #endif// DEBUG
}