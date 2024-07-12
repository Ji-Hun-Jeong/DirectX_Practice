#include "Header.hlsli"
cbuffer CubeMapConstant : register(b0)
{
    matrix model;
    matrix invTranspose;
}
cbuffer ViewProj : register(b1)
{
    matrix viewProj;
}
PSInput main(VSInput input)
{
    PSInput output;
    float4 pos = float4(input.position, 1.0f);
    pos = mul(pos, model);
    output.posWorld = pos;
    pos = mul(pos, viewProj);
    output.posProj = pos;
    
    float4 normal = float4(input.normal, 0.0f);
    normal = mul(normal, invTranspose);
    output.normal = normalize(normal);
    
    output.uv = input.uv;
    return output;
}