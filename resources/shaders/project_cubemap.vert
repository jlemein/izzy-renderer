#version 460

/**
 * Passes through vertex data as is. No manipulation of vertex data.
 * Useful for screen aligned quads that are defined using screen space coordinates.
 */
layout(location = 0) in vec2 aPos;
layout(location = 2) in vec3 aUv;

layout(location = 0) out vec3 out_uv;

void main() {
    out_uv = aUv;
    gl_Position = vec4(aPos, 0.0, 1.0);

}