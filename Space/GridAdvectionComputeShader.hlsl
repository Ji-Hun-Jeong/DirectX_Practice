#include "GridSimulationHeader.hlsli"
RWTexture2D<float4> g_density : register(u0);
RWTexture2D<float2> g_velocity : register(u1);

Texture2D<float4> g_densityTemp : register(t0);
Texture2D<float2> g_velocityTemp : register(t1);

[numthreads(32, 32, 1)]
void main( uint3 dtID : SV_DispatchThreadID )
{
    //float2 velocity = g_velocity[dtID.xy];
    //uint2 samplePos = dtID.xy - velocity;
    //g_density[dtID.xy] = g_densityTemp[samplePos];
    //g_densityTemp[dtID.xy] = g_density[dtID.xy];
    uint width, height;
    g_density.GetDimensions(width, height);
    float2 dx = float2(1.0f / width, 1.0f / height);
    float2 pos = (dtID.xy + 0.5f) * dx;

    float2 samplePos = pos - g_velocityTemp.SampleLevel(g_linearSampler, pos, 0) * dt * 0.01f;
    g_density[dtID.xy] = g_densityTemp.SampleLevel(g_linearSampler, samplePos, 0);
    g_velocity[dtID.xy] = g_velocityTemp.SampleLevel(g_linearSampler, samplePos, 0);
}