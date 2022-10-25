#version 460
#extension GL_ARB_separate_shader_objects : enable

#define EPSILON 0.00001

layout(binding = 1)
uniform ModelViewProjection {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPosition;// position of camera in world coordinates
};

layout(binding = 3)
uniform ConstantLight {
    vec4 color;
    float intensity;
    float radius;
};

#ifdef DEPTHPEELING
layout(std140, binding = 4)
uniform DepthPeeling {
    bool isPeelingPass;
    ivec2 screenSize;
};

layout(binding = 0) uniform sampler2D depthMap0; // opaque objects' depth map.
layout(binding = 1) uniform sampler2D depthMap1; // transparent objects' depth map (peeled depth).
layout(binding = 2) uniform sampler2D colorMap;  // combined transparent objects' color contribution.
#endif// DEPTHPEELING

layout(location = 0) in vec4 in_normal;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_tangent;
layout(location = 3) in vec3 in_btangent;

layout(location = 0) out vec4 outColor;

void main() {
#ifdef DEPTHPEELING
    vec2 uv = gl_FragCoord.xy / screenSize;

    if (isPeelingPass) {
        float depth0 = texture(depthMap0, uv).r;
        float depth1 = texture(depthMap1, uv).r;

        if (gl_FragCoord.z < depth1 + EPSILON || gl_FragCoord.z > depth0) {
            discard;
        }
    }
#endif
    outColor = vec4(color.rgb*intensity, color.a);
#ifdef DEPTHPEELING
    if (isPeelingPass) {
        vec4 color_d = texture(colorMap, uv);

        // blend under-operator, from Real-Time Rendering 4th edition (p.153)
        // I believe there is an error in the book and this one should be correct.
        outColor.rgb = /*color_d.a * */ color_d.rgb + (1.0 - color_d.a) * outColor.a * outColor.rgb;
        outColor.a = outColor.a * (1-color_d.a) + color_d.a;
    }
#endif
}