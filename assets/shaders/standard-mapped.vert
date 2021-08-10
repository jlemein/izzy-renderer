#version 460

#define MAX_LIGHTS 4

layout(std140, binding=2)
uniform Lighting {
// Positions in world space
    vec4 uPositions[MAX_LIGHTS];
    vec4 uColors[MAX_LIGHTS];
    float uIntensity[MAX_LIGHTS];
    float uAttenuation[MAX_LIGHTS];
    int uNumberLights;
};

layout(binding = 1)
uniform UniformBufferBlock {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPosition; // position of camera in world coordinates
};


// Model attributes
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUv;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

layout(location = 0) out vec4 out_normal;
layout(location = 1) out vec2 out_uv;
layout(location = 2) out vec4 out_TangentLightPosition; // ray towards light
layout(location = 3) out vec3 out_TangentViewPosition; // ray towards camera
layout(location = 4) out vec3 out_TangentFragPosition; // world position of shaded point

layout(location = 5) out vec3 out_tangent;
layout(location = 6) out vec3 out_btangent;

void main() {
    out_tangent = aTangent;
    out_btangent = aBitangent;

    vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
    vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
    mat3 TBN = transpose(mat3(T, B, N));

    out_TangentFragPosition = TBN * vec3(model * vec4(aPos, 1.0F));
    out_TangentViewPosition = TBN * viewPosition;
    out_TangentLightPosition = vec4(TBN * uPositions[0].xyz, 0.0F);

    mat4 MVP = proj * view * model;
    mat4 MV = view * model;
    mat4 invTranspose = inverse(transpose(model));

    vec4 world_position = model * vec4(aPos, 1.0F);


    gl_Position = MVP * vec4(aPos, 1.0);
    out_uv = aUv;

    //   // for (int i=0; i<uNumberLights; i++) {
    //        if (uPositions[0].w != 0.0) {
    //            // point light
    ////            light_direction[i] = vec4((uPositions[i] - world_position).xyz, 1.0);
    //            out_TangentLightPosition = uPositions[0];//vec4(TBN * uPositions[0].xyz, 1.0F);
    //        } else {
    //            // directional light
    ////            light_direction[i] = uPositions[i];
    //            out_TangentLightPosition = vec4(TBN * uPositions[0].xyz, 0.0F);
    //        }
    //    //}
    out_normal = invTranspose * vec4(aNormal, 1.0);
}