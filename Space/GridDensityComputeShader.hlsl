#include "GridSimulationHeader.hlsli"
RWTexture2D<float4> g_density : register(u0);
RWTexture2D<float2> g_velocity : register(u1);

float smootherstep(float x, float edge0 = 0.0f, float edge1 = 1.0f)
{
    // Scale, and clamp x to 0..1 range
    x = clamp((x - edge0) / (edge1 - edge0), 0, 1);

    return x * x * x * (3 * x * (2 * x - 5) + 10.0f);
}
static const float mr = sqrt(1280 * 1280 + 960 * 960);
[numthreads(32, 32, 1)]
void main( uint3 dtID : SV_DispatchThreadID )
{
    uint2 mousePos = uint2(mouseX, mouseY);
    uint2 vec = dtID.xy - mousePos;
    g_density[dtID.xy] = max(0.0, g_density[dtID.xy] - 0.001);
    g_velocity[dtID.xy] *= 0.999f;
    // unsigned int라서 마우스 입력이 없을 경우 CPU 코드에서 i = -1
    // 오버플로우로 인해 width 보다 큰 값으로 설정
    int radius = 70;
    if (mouseX < 1280)
    {
        // (length > 50) -> (dist > 1) -> (scale == 0)
        float dist = length(float2(dtID.xy) - mousePos) / radius;
        float scale = smootherstep(1.0 - dist);
    
        // velocity는 모든 픽셀에 적용하지만 scale에 따라 원 내부만 적용됨
        g_velocity[dtID.xy] += velocity * scale;
        g_density[dtID.xy] += densityColor * scale;
    }
    //float radius = sqrt(vec.x * vec.x + vec.y * vec.y) / mr;
    //g_density[dtID.xy] = max(g_density[dtID.xy] - 0.01f, 0.0f);
    //if (radius < 0.05f)
    //{
    //    float scale = smootherstep(1 - radius * 20.0f);
    //    g_density[dtID.xy] += densityColor * scale;
    //    g_velocity[dtID.xy] += velocity * scale;
    //}
}