#include "GridSimulationHeader.hlsli"
RWTexture2D<float2> g_velocity : register(u0);
RWTexture2D<float> g_divergence : register(u1);
RWTexture2D<float> g_pressure : register(u2);
RWTexture2D<float> g_pressureTemp : register(u3);

[numthreads(32, 32, 1)]
void main( uint3 dtID : SV_DispatchThreadID )
{
    uint width, height;
    g_velocity.GetDimensions(width, height);
    uint2 left = uint2(dtID.x == 0 ? width - 1 : dtID.x - 1, dtID.y);
    uint2 right = uint2(dtID.x == width - 1 ? 0 : dtID.x + 1, dtID.y);
    uint2 up = uint2(dtID.x, dtID.y == height - 1 ? 0 : dtID.y + 1);
    uint2 down = uint2(dtID.x, dtID.y == 0 ? height - 1 : dtID.y - 1);
    
    // Divergence를 계산(밀도가 늘어나고 있는지 감소하고 있는지)
    float deltaU = g_velocity[right].x - g_velocity[left].x;
    float deltaV = g_velocity[up].y - g_velocity[down].y;
    g_divergence[dtID.xy] = (deltaU + deltaV) * 0.5f;
    
    // 매 프레임마다 Pressure를 반복적으로 재계산하기 때문에 0으로 초기화
    g_pressure[dtID.xy] = 0.0f;
    g_pressureTemp[dtID.xy] = 0.0f;
}