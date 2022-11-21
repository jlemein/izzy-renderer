#version 460

layout (location = 0) out vec4 out_color;

layout(location = 0) in vec3 in_uv;

uniform samplerCube cubemap; // cubemap texture sampler


void main()
{
    out_color = texture(cubemap, in_uv);
    out_color.w = 1.0;
}