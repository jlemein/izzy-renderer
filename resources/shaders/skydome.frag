#version 460

layout (location = 0) out vec4 out_color;

layout(location = 0) in vec3 in_pos;

uniform sampler2D environmentMap;

#define TWO_PI 6.28318530718
#define PI 3.1415926538
#define PI2 1.57079632679

void main()
{
    vec3 d = normalize(in_pos);
    float phi = d.x == 0.0 ? PI : atan(d.z, d.x);
    float theta = asin(d.y);
    vec2 uv = vec2(phi / TWO_PI, (theta + PI2)/PI);

    out_color = texture(environmentMap, uv);
//    out_color.xy = uv;
//    out_color.z = 0.1;
//    out_color.r = theta;
//    out_color.g = phi;
//    out_color.b = 0.1;
    out_color.w = 0.0;
}