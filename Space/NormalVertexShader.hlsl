#include "Header.hlsli"
cbuffer NormalConstant : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
    matrix invTranspose;
};
PSInput main(VSInput input) 
{
    PSInput output;
    float4 pos = float4(input.position, 1.0f);
    float4 normal = float4(input.normal, 0.0f);
    pos = mul(pos, model);
    output.normal = mul(normal, invTranspose);
    output.normal = normalize(output.normal);
    
    pos.xyz += output.normal * input.uv.x * 30.0f;
    output.posWorld = pos;
    pos = mul(pos, view);
    pos = mul(pos, projection);
    output.posProj = pos;
    output.uv = input.uv;
    return output;
}