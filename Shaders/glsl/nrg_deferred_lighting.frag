#version 460

layout (location = 0) in vec2 f_uv;

struct Light
{
    vec4 position;
    vec4 color;
};

layout (set = 0, binding = 0) uniform SceneCameraUniformData {
    mat4 view;
    mat4 proj;
    mat4 view_inverse;
    mat4 proj_inverse;
    vec4 eye;
} camera;

layout (set = 0, binding = 1) uniform SceneLightsUniformData {
    Light lights[];
};

layout (set = 0, binding = 2) uniform sampler2D u_position;
layout (set = 0, binding = 3) uniform sampler2D u_normal;
layout (set = 0, binding = 4) uniform sampler2D u_albedo;

layout (push_constant) uniform DeferredLightingPushConstant {
    ivec4 params;  // [ No. Lights, -, -, - ]
} pc;

layout (location = 0) out vec4 outColor;

vec3 compute_diffuse(vec3 color, vec3 light_dir, vec3 normal) {
    float dot_nl = max(dot(normal, light_dir), 0.0);
    vec3 c = color * dot_nl;
    c += 0.1 * color;  // Ambient
    return c;
}

vec3 compute_specular(vec3 color, vec3 view_dir, vec3 light_dir, vec3 normal) {
    const float k_pi = 3.14159265;
    const float k_shininess = 2.5;

    const float k_energy_conservation = (2.0 + k_shininess) / (2.0 * k_pi);
    vec3 V = normalize(-view_dir);
    vec3 R = reflect(-light_dir, normal);
    float specular = k_energy_conservation * pow(max(dot(V, R), 0.0), k_shininess);

    return vec3(0.25 * specular);
}

void main() {
    vec2 uv = f_uv;
    uv.y = -f_uv.y;

    vec3 i_worldPos     = texture(u_position, uv).rgb;
    vec3 i_worldNormal  = texture(u_normal, uv).rgb;
    vec3 i_color        = texture(u_albedo, uv).rgb;

    vec3 i_viewDir      = camera.eye.xyz - i_worldPos;

    vec3 N = normalize(i_worldNormal);

    Light first_light = lights[0];
    vec4 light_pos = first_light.position;

    vec3 l_dir = light_pos.xyz - i_worldPos;

    vec3 L = normalize(l_dir);
    float light_distance = length(l_dir);

    vec3 diffuse = compute_diffuse(i_color, L, N);
    vec3 specular = compute_specular(i_color, i_viewDir, L, N);

    vec4 color = vec4(diffuse + specular, 1);

    vec3 origin = i_worldPos;
    vec3 direction = L;
    float t_min = 0.01;
    float t_max = light_distance;

    float gamma = 1.0 / 2.2;
    outColor = vec4(pow(color.rgb, vec3(gamma)), 1.0);
}
