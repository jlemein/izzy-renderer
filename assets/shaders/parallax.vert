#version 460

#define MAX_LIGHTS 4

layout(std140, binding=2)
uniform Lighting {
// Positions in world space
    vec4 lightPositions[MAX_LIGHTS];
    vec4 lightColors[MAX_LIGHTS];
    float lightIntensity[MAX_LIGHTS];
    float lightAttenuation[MAX_LIGHTS];
    int lightCount;
};

layout(binding = 1)
uniform UniformBufferBlock {
    mat4 model;
    mat4 view;
    mat4 projection;
};

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

// VS_OUT
layout(location = 0) out vec3 outFragPos;
layout(location = 1) out vec2 outTexCoords;
layout(location = 2) out vec3 outTangentLightPos[MAX_LIGHTS];
layout(location = 3) out vec3 outTangentViewPos;
layout(location = 4) out vec3 outTangentFragPos;

//// if direction.w is 0 - directional light
//// otherwise point light
//layout(location = 2) out vec4 light_direction[MAX_LIGHTS];

void main() {
    gl_Position = projection * view * model * vec4(aPosition, 1.0);
    outFragPos = vec3(model * vec4(aPosition, 1.0));
    outTexCoords = aTexCoords;

    vec3 T = normalize(mat3(model) * aTangent);
    vec3 B = normalize(mat3(model) * aBitangent);
    vec3 N = normalize(mat3(model) * aNormal);
    mat3 TBN = transpose(T, B, N);

    outTangentLightPos = TBN * lightPos;
    outTangentViewPos = TBN * viewPos;
    outTangentFragPos = TBN * outFragPos;

    //mat4 invTranspose = inverse(transpose(model));

    vec4 world_position = model * vec4(position, 1.0F);


    out_uv = uv;

    for (int i=0; i<lightCount; i++) {
        if (lightPositions[i].w != 0.0) {
            // point light
            light_direction[i] = vec4((uPositions[i] - world_position).xyz, 1.0);
        } else {
            // directional light
            light_direction[i] = uPositions[i];
        }
    }
    //normalOut = invTranspose * vec4(normal, 1.0);
}