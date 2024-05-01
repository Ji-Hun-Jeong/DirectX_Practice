#include "Header.hlsli"
cbuffer NormalConstant : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
    matrix invTranspose;
    float normalSize;
    float3 dummy;
};
GSInput main(VSInput input) 
{
    GSInput output;
    float4 pos = float4(input.position, 1.0f);
    float4 normal = float4(input.normal, 0.0f);
    pos = mul(pos, model);
   
    normal = mul(normal, invTranspose);
    normal = normalize(normal);
    
    output.position = pos.xyz;
    output.normal = normal;
     
    output.uv.xy = 0.0f;
    return output;
}