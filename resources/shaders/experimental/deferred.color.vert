#version 460

layout(std140, binding = 0)
uniform AlbedoSpecularity {
    vec4 albedo_specularity;
};

layout(std140, binding = 1)
uniform ModelViewProjection {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPosition; // position of camera in world coordinates
};

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUv;
layout(location = 3) in vec3 aTangent;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec2 out_uv;
layout(location = 3) out vec3 out_tangent;

void main() {
    mat4 MVP = proj * view * model;
//    mat4 invTranspose = inverse(transpose(model));

    vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
    vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));

    // all attributes to world space
    out_position = vec3(model * vec4(aPos, 1.0));
    out_normal = N.xyz;
    out_tangent = T.xyz;
    out_uv = aUv;

    //    out_normal = invTranspose * vec4(aNormal, 1.0);
    //    out_tangent = (model * vec4(aTangent, 1.0)).xyz;

    gl_Position = MVP * vec4(aPos, 1.0);
}