#include "Header.hlsli"
cbuffer Constant : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
    matrix invTranspose;
    float3 viewDir;
    float arrowSize = 1.0f;
    float3 upDir;
    float dummy1 = 0.0f;
    float3 rightDir;
    float dummy2 = 0.0f;
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