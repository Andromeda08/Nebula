struct PSInput
{
    float4 Position : SV_Position;
    float2 UV       : TEXCOORD0;
};

PSInput VSmain(uint VertexIndex : SV_VertexID)
{
    PSInput output;
    output.UV = float2((VertexIndex << 1) & 2, VertexIndex & 2);
    output.Position = float4(output.UV * 2.0f - 1.0f, 0.0f, 1.0f);
    return output;
}

float4 PSmain(PSInput input) : SV_Target
{
    float2 uv = input.UV;
    return float4(uv.x, 0.0, uv.y, 1.0);
}