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
};

struct Vertex {
    vec4 position;
};

struct StrandDesc {
    ivec4 desc;
};