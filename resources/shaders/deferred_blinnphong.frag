#version 460

// hardcoded. Cannot add or change order, or change names
layout (location = 0) out vec4 gbuffer_position;
layout (location = 1) out vec4 gbuffer_normal;
layout (location = 2) out vec4 gbuffer_albedospec;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in vec3 in_tangent;

layout(binding = 2) uniform sampler2D normalMap;
layout(binding = 3) uniform sampler2D albedoMap;

layout(std140, binding = 4)
uniform BlinnPhong {
    vec4 specular_color;
    float shininess;
};

void main()
{
    // create TBN matrix (tangent to world space)
    // needed to map the normal map data (in tangent space) to world space.
    vec3 T = normalize(in_tangent);
    vec3 N = normalize(in_normal);
    vec3 B = cross(T, N);
    mat3 TBN = mat3(T, B, N);

    // store the fragment position vector in the first gbuffer texture
    gbuffer_position = vec4(in_position, 1);

    vec3 normal = texture(normalMap, in_uv).rgb;
    normal = normalize(normal * 2.0 - 1.0); // transform normal vector to range [-1,1]
    gbuffer_normal = vec4(TBN * normal, 0);

    vec4 material_color = texture(albedoMap, in_uv);
    material_color.w = 0.0;
    gbuffer_albedospec = material_color;
}