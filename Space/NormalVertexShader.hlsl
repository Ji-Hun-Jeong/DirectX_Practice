#include "Header.hlsli"
cbuffer MeshConstant : register(b2)
{
    matrix world;
    matrix worldIT;
};

GSInput main(VSInput input) 
{
    GSInput output;
    float4 pos = float4(input.position, 1.0f);
    float4 normal = float4(input.normal, 0.0f);
    pos = mul(pos, world);
   
    normal = mul(normal, worldIT);
    normal = normalize(normal);
    
    output.position = pos.xyz;
    output.normal = normal;
    output.tangent = input.tangent;
     
    output.uv.xy = 0.0f;
    return output;
}