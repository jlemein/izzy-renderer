#version 460

layout(std140, binding = 1)
uniform ModelViewProjection {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPosition; // position of camera in world coordinates
};

layout(location = 0) in vec3 aPos;

layout(location = 0) out vec3 out_uv;

void main() {
    out_uv = aPos;

    mat4 view_no_translate = mat4(mat3(view));
    vec4 pos = proj * view_no_translate * vec4(aPos, 1.0);
    gl_Position =  pos;
}