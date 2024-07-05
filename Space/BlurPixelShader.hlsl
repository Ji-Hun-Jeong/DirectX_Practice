#include "Header.hlsli"
Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);
cbuffer PixelConstant : register(b0)
{
    float3 eyePos;
    int isSun;
    Light light;
    Material mat;
    Bloom bloom;
    Rim rim;
}
float4 main(PSInput input) : SV_TARGET
{
    float3 color = 0.0f;
    float2 uv = input.uv;
    float x = bloom.dx;
    float y = bloom.dy;

    // Take 13 samples around current texel:
    // a - b - c
    // - j - k -
    // d - e - f
    // - l - m -
    // g - h - i
    // === ('e' is the current texel) ===
    float3 a = g_texture.Sample(g_sampler, float2(uv.x - 2 * x, uv.y + 2 * y)).rgb;
    float3 b = g_texture.Sample(g_sampler, float2(uv.x, uv.y + 2 * y)).rgb;
    float3 c = g_texture.Sample(g_sampler, float2(uv.x + 2 * x, uv.y + 2 * y)).rgb;
                                                  
    float3 d = g_texture.Sample(g_sampler, float2(uv.x - 2 * x, uv.y)).rgb;
    float3 e = g_texture.Sample(g_sampler, float2(uv.x, uv.y)).rgb;
    float3 f = g_texture.Sample(g_sampler, float2(uv.x + 2 * x, uv.y)).rgb;
                                                  
    float3 g = g_texture.Sample(g_sampler, float2(uv.x - 2 * x, uv.y - 2 * y)).rgb;
    float3 h = g_texture.Sample(g_sampler, float2(uv.x, uv.y - 2 * y)).rgb;
    float3 i = g_texture.Sample(g_sampler, float2(uv.x + 2 * x, uv.y - 2 * y)).rgb;
                                                  
    float3 j = g_texture.Sample(g_sampler, float2(uv.x - x, uv.y + y)).rgb;
    float3 k = g_texture.Sample(g_sampler, float2(uv.x + x, uv.y + y)).rgb;
    float3 l = g_texture.Sample(g_sampler, float2(uv.x - x, uv.y - y)).rgb;
    float3 m = g_texture.Sample(g_sampler, float2(uv.x + x, uv.y - y)).rgb;

    // Apply weighted distribution:
    // 0.5 + 0.125 + 0.125 + 0.125 + 0.125 = 1
    // a,b,d,e * 0.125
    // b,c,e,f * 0.125
    // d,e,g,h * 0.125
    // e,f,h,i * 0.125
    // j,k,l,m * 0.5
    // This shows 5 square areas that are being sampled. But some of them overlap,
    // so to have an energy preserving downsample we need to make some adjustments.
    // The weights are the distributed, so that the sum of j,k,l,m (e.g.)
    // contribute 0.5 to the final color output. The code below is written
    // to effectively yield this sum. We get:
    // 0.125*5 + 0.03125*4 + 0.0625*4 = 1
    color = e * 0.125;
    color += (a + c + g + i) * 0.03125;
    color += (b + d + f + h) * 0.0625;
    color += (j + k + l + m) * 0.125;
    return float4(color, 1.0f);
}