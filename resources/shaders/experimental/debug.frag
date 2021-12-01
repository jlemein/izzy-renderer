#version 460
#extension GL_ARB_separate_shader_objects : enable

//layout(location = 0) in vec3 inNormal;
//layout(location = 1) in vec2 uv1;

layout(std140, binding = 0)
uniform ColorBlock {
    vec4 uColor;
    bool uDiffuseShading;
};

layout(location = 0) out vec4 outColor;

//layout(binding = 0) uniform sampler2D diffuseTexture;

void main() {
    if (uDiffuseShading) {
        vec3 nn_light_direction = normalize(vec3(1.0F, 1.0F, 0.0));
        float dot_normal_light = dot(nn_light_direction, normalize(vec3(0.0F, 1.0F, 0.0F)));

        outColor = dot_normal_light * uColor;
    } else {
        outColor = vec4(uColor);
    }

}