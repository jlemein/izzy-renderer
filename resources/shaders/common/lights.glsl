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