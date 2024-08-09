#version 460

struct CameraData {
    mat4 view;
    mat4 proj;
    mat4 view_inverse;
    mat4 proj_inverse;
    vec4 eye;
};

struct ObjectData {
    mat4 model;
    vec4 color;
};

layout (set = 0, binding = 0) uniform PrePassUniform {
    CameraData current;
    CameraData previous;
} camera;

layout (push_constant) uniform ObjectPushConstantData {
    ObjectData data;
} object;

layout (location = 0) in vec3 i_position;
layout (location = 1) in vec3 i_normal;
layout (location = 2) in vec2 i_uv;

layout (location = 0) out vec3 o_worldPos;
layout (location = 1) out vec3 o_worldNormal;
layout (location = 2) out vec2 o_uv;
layout (location = 3) out vec3 o_color;
layout (location = 4) out vec3 o_viewDir;
layout (location = 5) out vec4 o_currentPosition;
layout (location = 6) out vec4 o_previousPosition;

void main()
{
    CameraData current_camera  = camera.current;
    CameraData previous_camera = camera.previous;
    ObjectData obj             = object.data;

    vec3 origin = vec3(current_camera.view_inverse * vec4(0, 0, 0, 1));
    vec4 currentWorldPosition = obj.model * vec4(i_position, 1.0);

    o_worldPos = currentWorldPosition.xyz;
    o_worldNormal = mat3(obj.model) * i_normal;
    o_uv = i_uv;
    o_color = obj.color.xyz;
    o_viewDir = vec3(o_worldPos - origin);

    o_currentPosition = current_camera.proj * current_camera.view * currentWorldPosition;
    o_previousPosition = previous_camera.proj * previous_camera.view * obj.model * vec4(i_position, 1.0);
    o_previousPosition = o_currentPosition;
    
    gl_Position = o_currentPosition;
}