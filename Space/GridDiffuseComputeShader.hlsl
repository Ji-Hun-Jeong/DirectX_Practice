#include "GridSimulationHeader.hlsli"
RWTexture2D<float4> g_density : register(u0);
RWTexture2D<float4> g_densityTemp : register(u1);
RWTexture2D<float2> g_velocity : register(u2);
RWTexture2D<float2> g_velocityTemp : register(u3);
[numthreads(32, 32, 1)]
void main( uint3 dtID : SV_DispatchThreadID )
{
    uint width, height;
    g_velocity.GetDimensions(width, height);
    
    uint2 left = uint2(dtID.x == 0 ? width - 1 : dtID.x - 1, dtID.y);
    uint2 right = uint2(dtID.x == width - 1 ? 0 : dtID.x + 1, dtID.y);
    uint2 up = uint2(dtID.x, dtID.y == height - 1 ? 0 : dtID.y + 1);
    uint2 down = uint2(dtID.x, dtID.y == 0 ? height - 1 : dtID.y - 1);
    
    g_density[dtID.xy] = (g_densityTemp[dtID.xy] + viscosity * dt * (
    g_densityTemp[left] +
    g_densityTemp[right] +
    g_densityTemp[up] +
    g_densityTemp[down])) / (1 + 4 * viscosity * dt);
    
    g_velocity[dtID.xy] = (g_velocityTemp[dtID.xy] + viscosity * dt * (
    g_velocityTemp[left] +
    g_velocityTemp[right] +
    g_velocityTemp[up] +
    g_velocityTemp[down])) / (1.0f + 4.0f * viscosity * dt);
}