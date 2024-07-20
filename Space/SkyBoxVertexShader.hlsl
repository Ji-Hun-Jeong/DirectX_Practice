#include "Header.hlsli"
cbuffer MeshConstant : register(b2)
{
    matrix world;
    matrix worldIT;
};
PSInput main(VSInput input)
{
    PSInput output;
    float4 pos = float4(input.position, 1.0f);
    pos = mul(pos, world);
    output.posWorld = pos;
    pos = mul(pos, viewProj);
    output.posProj = pos;
    
    float4 normal = float4(input.normal, 0.0f);
    normal = mul(normal, worldIT);
    output.normal = normalize(normal);
    
    output.uv = input.uv;
    return output;
}