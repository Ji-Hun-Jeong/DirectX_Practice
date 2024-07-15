#include "Header.hlsli"
cbuffer MeshConstant : register(b2)
{
    matrix model;
    matrix invTranspose;
    float3 viewDir;
    float arrowSize = 1.0f;
    float3 upDir;
    float dummy3 = 0.0f;
    float3 rightDir;
    float dummy4 = 0.0f;
};
GSInput main(VSInput input) 
{
    GSInput output;
    float4 origin = float4(0.0f, 0.0f, 0.0f, 1.0f);
    output.position = origin;
    output.normal = input.normal;
    output.uv = input.uv;
    return output;
}