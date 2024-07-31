#include "Header.hlsli"
struct Particle
{
    float3 pos;
    float3 color;
};
RWStructuredBuffer<Particle> g_sb : register(u0);

[numthreads(256, 1, 1)]
void main(uint3 gtId : SV_GroupID, uint3 dtId : SV_DispatchThreadID)
{
    float3 pos = g_sb[dtId.x].pos;
    const float dt = 0.01f;
    float3 velocity = float3(-pos.y, pos.x, 0.0f);
    pos += velocity * dt;
    g_sb[dtId.x].pos = pos;
}