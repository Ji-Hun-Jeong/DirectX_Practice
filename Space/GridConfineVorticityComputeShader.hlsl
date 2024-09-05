#include "GridSimulationHeader.hlsli"
RWTexture2D<float2> g_velocity : register(u0);
RWTexture2D<float> g_vorticity : register(u1);
[numthreads(32, 32, 1)]
void main( uint3 dtID : SV_DispatchThreadID )
{
    uint width, height;
    g_vorticity.GetDimensions(width, height);
    float2 dx = float2(1.0f / width, 1.0f / height);
    
    uint2 left = uint2(dtID.x == 0 ? width - 1 : dtID.x - 1, dtID.y);
    uint2 right = uint2(dtID.x == width - 1 ? 0 : dtID.x + 1, dtID.y);
    uint2 up = uint2(dtID.x, dtID.y == height - 1 ? 0 : dtID.y + 1);
    uint2 down = uint2(dtID.x, dtID.y == 0 ? height - 1 : dtID.y - 1);
    
    float2 n = float2((abs(g_vorticity[right]) - abs(g_vorticity[left])) / (2.0f * dx.x),
    (abs(g_vorticity[up]) - abs(g_vorticity[down])) / (2.0f * dx.y));
    
    if (length(n) < 1e-5)
        return;
    
    float3 N = float3(normalize(n), 0.0f);
    float3 w = float3(0.0f, 0.0f, g_vorticity[dtID.xy]);
    
    g_velocity[dtID.xy] += cross(N, w).xy * dx * 0.2f;

}