#version 460

layout (location = 0) out vec4 out_color;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;

layout(binding = 0) uniform sampler2D gbuffer_position;
layout(binding = 1) uniform sampler2D gbuffer_normal;
layout(binding = 2) uniform sampler2D gbuffer_albedospec;

void main()
{
    //    out_color = texture(gbuffer_position, 2*in_uv); //vec4(1, 0, 0, 0);
    // store the fragment position vector in the first gbuffer texture
    if ((in_uv.x > 0.4995 && in_uv.x < 0.5005) || (in_uv.y >0.4995 && in_uv.y <= 0.5005)) {
        out_color = vec4(0.125);
        return;
    }
    if (in_uv.x < 0.5 && in_uv.y >= 0.5) {
        out_color = texture(gbuffer_position, in_uv*2.0);
    } else if (in_uv.x >= 0.5 && in_uv.y >= 0.5) {
        out_color = texture(gbuffer_normal, (in_uv-0.5)*2.0);
    } else if (in_uv.x < 0.5 && in_uv.y < 0.5) {
        vec2 uv = vec2(in_uv.x*2.0, in_uv.y*2.0);
        out_color = vec4(texture(gbuffer_albedospec, uv).rgb, 1);
    } else {
        // specular color
        vec2 uv = vec2((in_uv.x-0.5)*2.0, in_uv.y*2.0);
        out_color = vec4(texture(gbuffer_albedospec, uv).w);
    }
}