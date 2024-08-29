#include "GridSimulationHeader.hlsli"
RWTexture2D<float> g_divergence : register(u0);
RWTexture2D<float> g_pressure : register(u1);
RWTexture2D<float> g_pressureOut : register(u2);

[numthreads(32, 32, 1)]
void main( uint3 dtID : SV_DispatchThreadID )
{
    uint width, height;
    g_divergence.GetDimensions(width, height);
    uint2 left = uint2(dtID.x == 0 ? width - 1 : dtID.x - 1, dtID.y);
    uint2 right = uint2(dtID.x == width - 1 ? 0 : dtID.x + 1, dtID.y);
    uint2 up = uint2(dtID.x, dtID.y == height - 1 ? 0 : dtID.y + 1);
    uint2 down = uint2(dtID.x, dtID.y == 0 ? height - 1 : dtID.y - 1);
    
    g_pressureOut[dtID.xy] = (-g_divergence[dtID.xy]
    + g_pressure[left] + g_pressure[right] + g_pressure[up] + g_pressure[down]) * 0.25f;
}