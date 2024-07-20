#include "Header.hlsli"
cbuffer MeshConstant : register(b2)
{
    matrix world;
    matrix worldIT;
};
float4 main(VSInput input) : SV_POSITION
{
    float4 pos = float4(input.position, 1.0f);
    pos = mul(pos, world);
    pos = mul(pos, viewProj);
    return pos;
}