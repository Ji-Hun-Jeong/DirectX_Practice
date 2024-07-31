#include "Header.hlsli"
struct Particle
{
    float3 pos;
    float3 color;
    float3 velocity;
    float lifeTime;
    float radius;
};

cbuffer AnimateConstData : register(b0)
{
    float2 mousePos;
    float dt;
    float Dummy;
}

RWStructuredBuffer<Particle> g_sb : register(u0);
groupshared uint newCount = 10;

[numthreads(256, 1, 1)]
void main(uint3 gtId : SV_GroupID, uint3 dtId : SV_DispatchThreadID)
{
    
}