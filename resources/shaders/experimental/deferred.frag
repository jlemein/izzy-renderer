#version 460

layout (location = 0) out vec4 gbuffer_position;
layout (location = 1) out vec4 gbuffer_normal;
layout (location = 2) out vec4 gbuffer_tangent;
layout (location = 3) out vec4 gbuffer_albedospec;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in vec3 in_tangent;

layout(binding = 0) uniform sampler2D texture_diffuse1;
layout(binding = 1) uniform sampler2D texture_specular1;
layout(binding = 2) uniform sampler2D normalMap;
layout(binding = 3) uniform sampler2D albedoMap;

void main()
{
    // create TBN matrix (tangent to world space)
    // needed to map the normal map data (in tangent space) to world space.
    vec3 T = normalize(in_tangent);
    vec3 N = normalize(in_normal);
    vec3 B = cross(T, N);
    mat3 TBN = transpose(mat3(T, B, N));

    gbuffer_position = vec4(in_position, 1);
    gbuffer_normal = vec4(TBN * in_normal, 0);

    vec4 material_color = texture(albedoMap, in_uv);
    //    materialcolor.w = albedo_specularity;
    gbuffer_albedospec = material_color;

    // also store the per-fragment normals into the gbuffer
//    gNormal = normalize(in_normal);
//    // and the diffuse per-fragment color
//    gAlbedoSpec.rgb = texture(texture_diffuse1, in_uv).rgb;
//    // store specular intensity in gAlbedoSpec's alpha component
//    gAlbedoSpec.a = texture(texture_specular1, in_uv).r;
}