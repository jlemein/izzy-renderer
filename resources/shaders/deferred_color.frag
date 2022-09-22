#version 460

layout(std140, binding = 0)
uniform AlbedoSpecularity {
    vec4 albedo_specularity;
};

layout(std140, binding = 1)
uniform ModelViewProjection {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPosition;// position of camera in world coordinates
};

// hardcoded. Cannot add or change order, or change names
layout (location = 0) out vec4 gbuffer_position;
layout (location = 1) out vec4 gbuffer_normal;
layout (location = 2) out vec4 gbuffer_albedospec;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in vec3 in_tangent;

void main()
{
    // create TBN matrix (tangent to world space)
    // needed to map the normal map data (in tangent space) to world space.
    vec3 T = normalize(in_tangent);
    vec3 N = normalize(in_normal);
    vec3 B = cross(T, N);
    mat3 TBN = transpose(mat3(T, B, N));

    gbuffer_position = vec4(in_position, 1);
    gbuffer_normal = vec4(N, 0);
    gbuffer_albedospec = albedo_specularity;
}