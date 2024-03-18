#define WORKGROUP_SIZE 32
#define PI 3.14159265

struct PushConstant
{
    float dt;
    float pad1, pad2, pad3;
    int   vertex_count;
    int   pad5, pad6, pad7;
};

[[vk::push_constant]] ConstantBuffer<PushConstant> constants;

struct Vertex
{
    float4 position;
};

[[vk::binding(0)]] RWStructuredBuffer<Vertex> current_position;
[[vk::binding(1)]] RWStructuredBuffer<Vertex> future_position;

[numthreads(WORKGROUP_SIZE, 1, 1)]
void main(uint3 groupID : SV_GroupID, uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint localID  = dispatchThreadID.x;
    uint globalID = groupID.x;
    
    uint local_i = floor(constants.vertex_count / (WORKGROUP_SIZE * 1024));
    for (uint i = 0; i < local_i; i++)
    {
        uint offset = (globalID * WORKGROUP_SIZE + localID) * local_i + i;
        float4 position = current_position[offset].position + (float4(10000, 0, 0, 0) * sin(PI * constants.dt));
        future_position[offset].position = position;
    }
}