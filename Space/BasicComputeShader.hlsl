#include "Header.hlsli"
struct Particle
{
    float3 pos;
    float3 color;
};
RWTexture2D<float4> g_uav : register(u0);
RWStructuredBuffer<Particle> g_sb : register(u1);

[numthreads(32, 32, 1)]
void main(uint3 gtId : SV_GroupID, uint3 dtId : SV_DispatchThreadID)
{
    if (gtId.x % 2)
    {
        if (gtId.y % 2)
            g_uav[dtId.xy] = float4(1.0f, 0.0f, 0.0f, 1.0f);
        else
            g_uav[dtId.xy] = float4(0.0f, 0.0f, 1.0f, 1.0f);
    }
    else
    {
        if (gtId.y % 2)
            g_uav[dtId.xy] = float4(0.0f, 0.0f, 1.0f, 1.0f);
        else
            g_uav[dtId.xy] = float4(1.0f, 0.0f, 0.0f, 1.0f);
    }
}