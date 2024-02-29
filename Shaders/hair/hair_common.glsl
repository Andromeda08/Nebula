struct TaskOut {
    uint baseID;
    uint pps[31];
};

struct MeshOut {
    vec3 color;
};

struct Vertex {
    vec4 position;
    vec4 thickness;
};

struct StrandDescription {
    uint id;
    uint point_count;
    uint strandlet_count;
};
