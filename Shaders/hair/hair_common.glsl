#extension GL_EXT_shader_explicit_arithmetic_types_int8 : require

#ifndef WORKGROUP_SIZE
    #define WORKGROUP_SIZE 32
#endif

#ifndef DEBUG
    // #define DEBUG
#endif

struct Task {
    uint    baseID;
    uint8_t deltaID[WORKGROUP_SIZE - 1]; 
};

struct Mesh {
    vec4 color;
    vec4 world_position;
    vec4 world_tangent;
    vec2 uv;
};

struct MeshData {
    vec4 world_position;
    vec4 world_tangent;
};

struct Vertex {
    vec4 position;
};

struct StrandDesc {
    int strand_id;
    int vertex_count;
    int strandlet_count;
    int vertex_offset;
};

const int COLOR_COUNT = 12;
const vec3 color_pool[COLOR_COUNT] = {
    vec3(234, 118, 203), vec3(136, 57, 239), vec3(210, 15, 57), vec3(230, 69, 83),
    vec3(254, 100, 11), vec3(223, 142, 29), vec3(223, 142, 29), vec3(64, 160, 43),
    vec3(23, 146, 153), vec3(32, 159, 181), vec3(30, 102, 245), vec3(144, 135, 253)
};

vec4 get_meshlet_color(uint i)
{
    return vec4(color_pool[i % COLOR_COUNT] / 255, 1.0);
}
