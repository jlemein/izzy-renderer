#version 460

layout(std140, binding = 0)
uniform AlbedoSpecularity {
    vec4 albedo_specularity;
};

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_normal;
layout(location = 2) in vec2 in_uv;

void main()
{
    gPosition = vec4(in_position, 0);
    gNormal = vec4(in_normal.xyz, 1);
    gAlbedoSpec = albedo_specularity;
}