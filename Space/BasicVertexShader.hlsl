#include "Header.hlsli"
cbuffer Constant : register(b0)
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
    pos = mul(pos, model);
    output.posWorld = pos;
    pos = mul(pos, view);
    pos = mul(pos, projection);
    output.posProj = pos;
    
    float4 normal = float4(input.normal, 0.0f);
    normal = mul(normal, invTranspose);
    normal = normalize(normal);
    output.normal = normal.xyz;
    
    output.uv = input.uv;
    output.tangent = mul(float4(input.tangent, 0.0), model).xyz;
    
    return output;
}