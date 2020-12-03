#version 460
#extension GL_ARB_separate_shader_objects : enable

//layout(location = 0) in vec4 inNormal;
//layout(location = 1) in vec2 uv1;

layout(std140, binding = 0)
uniform ColorBlock {
    vec4 uColor;
};

layout(location = 0) out vec4 outColor;

//layout(binding = 0) uniform sampler2D diffuseTexture;

void main() {
//    const vec3 light_position = vec3(0.0, 2.5, 0.3);
    const vec3 color = uColor.xyz;
//
//    vec3 nn_lightdir = normalize(light_position);
//    vec3 nn_normal = normalize(inNormal).xyz;
//    float dot_normal_light = dot(nn_lightdir, nn_normal);

//    outColor = dot_normal_light * vec4(color, 1.0);
    outColor = vec4(color, 1.0);
}