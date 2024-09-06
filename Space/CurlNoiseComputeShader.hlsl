#include "NoiseHeader.hlsli"
RWTexture2D<float4> g_rtv : register(u0);
RWStructuredBuffer<Particle> g_particle : register(u1);

static float dt = 0.001f; // ConstBuffer로 받아올 수도 있음

float getNoise(float2 uv)
{
    // freq 여러가지로 바꿔보기
    return perlinfbm(float3(uv, 0.0f), 2.0, 1);
}

float2 getCurl(float2 uv, float2 dx)
{
    float2 dp;
    dp.x = (getNoise(uv + float2(dx.x, 0.0f)) - getNoise(uv - float2(dx.x, 0.0f))) / (2.0f * dx.x);
    dp.y = (getNoise(uv + float2(0.0f, dx.y)) - getNoise(uv - float2(0.0f, dx.y))) / (2.0f * dx.y);
    return float2(dp.y, -dp.x);
}

[numthreads(32, 32, 1)]
void main( uint3 dtID : SV_DispatchThreadID )
{
    uint width, height;
    g_rtv.GetDimensions(width, height);
    float2 dx = float2(1.0f / width, 1.0f / height);
    float2 uv = dtID.xy * dx;
    g_rtv[dtID.xy] = max(g_rtv[dtID.xy] - 0.1f, 0.0f);
    g_particle[dtID.x].pos.xy += getCurl(g_particle[dtID.x].pos.xy, dx) * dt;
}