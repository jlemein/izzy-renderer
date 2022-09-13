#version 460

#define MAX_POINT_LIGHTS 4
//#define DEBUG // define to show quad view (contents of gbuffer).

layout (location = 0) out vec4 out_color;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in vec3 in_TangentFragPosition;
layout(location = 4) in vec4 in_TangentPointLightPosition[MAX_POINT_LIGHTS];

layout(binding = 0) uniform sampler2D gbuffer_position;
layout(binding = 1) uniform sampler2D gbuffer_normal;
layout(binding = 2) uniform sampler2D gbuffer_tangent; // will be removed
layout(binding = 3) uniform sampler2D gbuffer_albedospec;

struct PointLight {
    vec4 worldPosition;
    vec4 color;
    float intensity;
    float radius;
};

layout(std140, binding = 1)
uniform ModelViewProjection {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPosition;// position of camera in world coordinates
};

layout(std140, binding=2)
uniform DeferredLighting {
    vec3 viewPos;
    int numberOfLights;
    PointLight pointLights[MAX_POINT_LIGHTS];
};


// Debug part
//const float AXIS_LINE_WIDTH = 0.001;
//const float[] TOP_X = {0.0, 0.33, 0.67 };
//const int BTM_X = 2;
//
//
//bool isInSquare(int x, int y, vec2 uv) {
//    float edge_x_start = TOP_X[x] - AXIS_LINE_WIDTH/2;
//    float edge_x_end = TOP_X[x] + AXIS_LINE_WIDTH/2;
//    float edge_y_start = TOP_Y[y] - AXIS_LINE_WIDTH/2;
//    float edge_y_end = TOP_Y[x] + AXIS_LINE_WIDTH/2;
//
//    return uv.x > edge_start && uv.x < edge_end && uv.y > edge_y_start && uv.y < edge_y_end;
//}
//
//vec2 transformUv(vec2 uv, int x, int y) {
//    vec2 tuv = vec2(uv.x - TOP_X[x], uv.y);
//}

vec4 debugview() {
    vec4 out_color = vec4(0);
    // splitter
    if ((in_uv.x > 0.4995 && in_uv.x < 0.5005) || (in_uv.y >0.4995 && in_uv.y <= 0.5005)) {
        return vec4(0.125);
    }
    if (in_uv.x < 0.5 && in_uv.y >= 0.5) {
        out_color = texture(gbuffer_position, in_uv*2.0);
    } else if (in_uv.x >= 0.5 && in_uv.y >= 0.5) {
        out_color = texture(gbuffer_normal, (in_uv-0.5)*2.0);
    } else if (in_uv.x < 0.5 && in_uv.y < 0.5) {
        // LEFT BOTTOM
        vec2 uv = vec2(in_uv.x*2.0, in_uv.y*2.0);
        out_color = vec4(texture(gbuffer_albedospec, uv).rgb, 1);
    } else {
        // RIGHT_BOTTOM
        vec2 uv = vec2((in_uv.x-0.5)*2.0, in_uv.y*2.0);
        out_color = texture(gbuffer_tangent, uv);
    }

    return out_color;
}

//#define DEBUG // define to show quad view (contents of gbuffer).
void main()
{
    #ifdef DEBUG
    out_color = debugview();
    #else // DEBUG

        // gbuffer layout
        // positions in world space.
        //   |-- normals in world space.
        //   |-- tangents in world space.
        // fetch normal and tangent from gBuffer.

        vec4 p = texture(gbuffer_position, in_uv);
        vec3 n = texture(gbuffer_normal, in_uv).rgb;
        vec4 albedo_spec = texture(gbuffer_albedospec, in_uv);

        vec3 view_position = viewPosition;//inverse(view)[3];// * vec4(0,0,0,1);
        vec3 v = normalize(view_position.xyz - p.xyz);
//        vec3 v = normalize(viewPosition.xyz - p.xyz);

    //        out_color = vec4(albedo_spec.rgb, 0.0);

        // convert light and view direction in tangent space
//        vec3 tsNormal = TBN * gbNormal;
//        vec3 tsView = TBN * viewPosition;
//        vec3 tsPosition = TBN * texture(gbuffer_position, in_uv).rgb;
//        vec3 tsViewDir = tsView - tsPosition;
//
//        // loop over the lights
        for (int i=0; i<numberOfLights; i++) {
            PointLight light = pointLights[i];
            vec3 l = light.worldPosition.xyz - p.xyz;
            l = normalize(l);
            float dist = length(l);

            float lambertian = max(0.0, dot(n, l));
            vec3 diffuse = albedo_spec.rgb * light.color.rgb * lambertian / (dist*dist);

            // compute specular intensity using half angle
            vec3 h = normalize(l + v);
            float ndoth = clamp(dot(n, h), 0.0, 1.0);
            float intensity = albedo_spec.w * pow(ndoth, 16.0);
            vec3 specular = intensity * light.color.rgb / (dist*dist);

            // add diffuse and specular component
            out_color.rgb += diffuse + specular;
        }

//            out_color = vec4((n + 1.0)/2.0, 0);
//
//        // normal (tangent space)
//aure(gbuffer_albedospec, in_uv).rgb;
//        float specular = texture(gbuffer_albedospec, in_uv).a;
//
//        vec3 lighting = albedo * 0.1;
//
//        for (int i=0; i<numberOfLights; ++i) {
//            PointLight pt = pointLights[i];
//            vec3 light_vector = in_TangentPointLightPosition[i].xyz;// - in_TangentFragPosition;
//    //        vec3 lightdir = normalize(pt.position.xyz - fragPos);
//
//            vec3 diffuse = light_vector;// max(0.0, dot(normal, normalize(light_vector))) * albedo * pt.color.rgb * pt.intensity;
//
//            lighting += diffuse;
//        }
//
//        out_color = vec4(lighting, 0.0);
        #endif
}