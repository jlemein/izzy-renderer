#define EPSILON 0.00001

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

#define DEPTH_PEEL_DISCARD_CHECK() {                                                                    \
    vec2 uv = gl_FragCoord.xy / screenSize;                                                             \
    if (isPeelingPass) {                                                                                \
        float depth0 = texture(depthMap0, uv).r;                                                        \
        float depth1 = texture(depthMap1, uv).r;                                                        \
                                                                                                        \
        if (gl_FragCoord.z < depth1 + EPSILON || gl_FragCoord.z > depth0) {                             \
            discard;                                                                                    \
        }                                                                                               \
    }                                                                                                   \
 }

#define DEPTH_PEEL_BLEND_OVER(){                                                                        \
    vec2 uv = gl_FragCoord.xy / screenSize;                                                             \
    if (isPeelingPass) {                                                                                \
        vec4 color_d = texture(colorMap, uv);                                                           \
                                                                                                        \
        /* blend under-operator, from Real-Time Rendering 4th edition (p.153) */                        \
        /* I believe there is an error in the book and this one should be correct. */                   \
        /* out_color.rgb = color_d.rgb + (1.0 - color_d.a) * out_color.a * out_color.rgb; */            \
        /* out_color.a = out_color.a * (1-color_d.a) + color_d.a; */                                    \
                                                                                                        \
        /* MAYBE NEXT ONE IS BETTER */                                                                  \
        /* blend under-operator, from Real-Time Rendering 4th edition (p.153) */                        \
        /* I believe there is an error in the book and this one should be correct. */                   \
        out_color.rgb = color_d.a * color_d.rgb + (1.0 - color_d.a) * out_color.rgb;                    \
        out_color.a = out_color.a * (1-color_d.a) + color_d.a;                                          \
    }                                                                                                   \
}