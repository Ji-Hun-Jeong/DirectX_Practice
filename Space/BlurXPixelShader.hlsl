#include "Header.hlsli"
Texture2D g_shaderResourceView : register(t0);
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
    float2 uv = input.uv;
    float3 color = float3(0.0f, 0.0f, 0.0f);
    for (int i = -2; i <= 2;++i)
    {
        color += g_shaderResourceView.Sample(g_sampler, float2(uv.x + bloom.dx * i, uv.y));
    }
    color = color / 5.0f;
    return float4(color, 1.0f);
}