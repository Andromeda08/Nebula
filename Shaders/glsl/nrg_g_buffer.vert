#version 460

layout (set = 0, binding = 0) uniform PrePassUniform {
    mat4 view;
    mat4 proj;
    mat4 view_inverse;
    mat4 proj_inverse;
    vec4 eye;
} camera;

layout (push_constant) uniform ObjectPushConstantData {
    mat4 model;
    vec4 color;
} obj;

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
    //CameraData camera = ubo.current_camera;
    //CameraData previous_camera = ubo.previous_camera;

    vec3 origin = vec3(camera.view_inverse * vec4(0, 0, 0, 1));
    vec4 currentWorldPosition = obj.model * vec4(i_position, 1.0);

    o_worldPos = currentWorldPosition.xyz;
    o_worldNormal = mat3(obj.model) * i_normal;
    o_uv = i_uv;
    o_color = obj.color.xyz;
    o_viewDir = vec3(o_worldPos - origin);

    o_currentPosition = camera.proj * camera.view * currentWorldPosition;
    //o_previousPosition = previous_camera.proj * previous_camera.view * obj.model * vec4(i_position, 1.0);
    o_previousPosition = o_currentPosition;
    
    gl_Position = o_currentPosition;
}