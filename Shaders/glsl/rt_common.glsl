// Common
struct Vertex
{
    vec3 position;
    vec3 normal;
    vec2 uv;
};

struct CameraData
{
    mat4 view;
    mat4 proj;
    mat4 view_inverse;
    mat4 proj_inverse;
    vec4 eye;
};

vec3 compute_diffuse(vec3 color, vec3 light_dir, vec3 normal) {
    float dot_nl = max(dot(normal, light_dir), 0.0);
    vec3 c = color * dot_nl;
    c += 0.1 * color;
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

// Ray Tracing
struct RayDescription
{
    vec4  origin;
    vec4  direction;
    float t_min;
    float t_max;
    uint  ray_flags;
};

struct RTPayloadBasic
{
    vec3 hit_value;
    vec3 ray_origin;
    vec3 ray_direction;
};

struct RTPayload
{
    vec3 hit_value;
    vec3 ray_origin;
    vec3 ray_direction;
    vec3 attenuation;
    int  depth;
    int  done;
};