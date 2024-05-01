#version 460

layout (location = 0) in vec3 i_position;
layout (location = 1) in vec3 i_normal;
layout (location = 2) in vec2 i_uv;

layout(set = 0, binding = 0) uniform CameraDataUniform {
    mat4 view;
    mat4 proj;
    mat4 view_inverse;
    mat4 proj_inverse;
    vec4 eye;
} camera;

layout(push_constant) uniform PushConstant {
    mat4 model;
    vec4 color;
} object;

void main() {
    gl_Position = camera.proj * camera.view * object.model * vec4(i_position, 1.0);
}