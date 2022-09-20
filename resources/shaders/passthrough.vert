#version 460

/**
 * Passes through vertex data as is. No manipulation of vertex data.
 * Useful for screen aligned quads that are defined using screen space coordinates.
 */
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUv;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec2 out_uv;

void main() {
    gl_Position = vec4(aPos, 1.0);
    out_position = aPos;
    out_uv = aUv;
}