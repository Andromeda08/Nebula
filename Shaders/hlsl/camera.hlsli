struct CameraData
{
    float4x4 view;
    float4x4 proj;
    float4x4 viewInverse;
    float4x4 projInverse;
    float4 eye;
    float nearPlane;
    float farPlane;
};