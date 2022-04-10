#version 460

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;

layout(binding = 0) uniform sampler2D texture_diffuse1;
layout(binding = 1) uniform sampler2D texture_specular1;

void main()
{
    // store the fragment position vector in the first gbuffer texture
    gPosition = vec4(1, 0, 0, 1);//in_position;
    // also store the per-fragment normals into the gbuffer
//    gNormal = normalize(in_normal);
//    // and the diffuse per-fragment color
//    gAlbedoSpec.rgb = texture(texture_diffuse1, in_uv).rgb;
//    // store specular intensity in gAlbedoSpec's alpha component
//    gAlbedoSpec.a = texture(texture_specular1, in_uv).r;
}