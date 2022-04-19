#version 460

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_normal;
layout(location = 2) in vec2 in_uv;

layout(binding = 0) uniform sampler2D texture_diffuse1;
layout(binding = 2) uniform sampler2D texture_specular1;
layout(binding = 3) uniform sampler2D normalMap;
layout(binding = 4) uniform sampler2D albedoMap;

layout(std140, binding = 4)
uniform BlinnPhong {
    vec4 specular_color;
    float shininess;
};

void main()
{
    vec3 surf_normal = normalize(texture(normalMap, in_uv).rgb*2.0-1.0);
    vec4 material_color = texture(albedoMap, in_uv);

    // store the fragment position vector in the first gbuffer texture
    gPosition = vec4(in_position, 1);//vec4(1, 0, 0, 1);//in_position;
    gNormal = vec4(surf_normal, 1);
    gAlbedoSpec = vec4(1,1, 0, 0);//material_color;

    // also store the per-fragment normals into the gbuffer
//    gNormal = normalize(in_normal);
//    // and the diffuse per-fragment color
//    gAlbedoSpec.rgb = texture(texture_diffuse1, in_uv).rgb;
//    // store specular intensity in gAlbedoSpec's alpha component
//    gAlbedoSpec.a = texture(texture_specular1, in_uv).r;
}