#version 460

layout (location = 0) out vec4 out_color;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;

layout(binding = 0) uniform sampler2D _texture;


void main()
{
    out_color = texture(_texture, in_uv);
}