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
} light;

layout (location = 0) out vec4 o_worldPos;
layout (location = 1) out vec4 o_color;

void main() {
    o_worldPos  = camera.proj * camera.view * light.model * vec4(i_position, 1.0);
    o_color     = light.color;
    gl_Position = o_worldPos;
}