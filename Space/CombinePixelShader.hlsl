#include "Header.hlsli"
Texture2D g_originalTexture : register(t0);
Texture2D g_blurTexture : register(t1);
SamplerState g_sampler : register(s0);
cbuffer PixelConstant : register(b0)
{
    float3 eyePos;
    int isSun;
    Light light;
    Material mat;
    Bloom bloom;
    Rim rim;
};
float4 main(PSInput input) : SV_TARGET
{
    float3 color;
    color = g_originalTexture.Sample(g_sampler, input.uv) + g_blurTexture.Sample(g_sampler, input.uv) * bloom.bloomStrength;
    return float4(color, 1.0f);
}