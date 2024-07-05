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
    
    int useAlbedo = false;
    int useNormal = false;
    int useAO = false;
    int useRoughness = false;

    int useMetallic = false;
    float exposure = 1.0f;
    float gamma = 1.0f;
    float dummy;
};

float3 ToneMapping(float3 color)
{
    float3 invGamma = float3(1, 1, 1) / gamma;
    color = clamp(exposure * color, 0., 1.);
    color = pow(color, invGamma);
    return color;
}

float4 main(PSInput input) : SV_TARGET
{
    float3 color1 = g_originalTexture.Sample(g_sampler, input.uv);
    float3 color2 = g_blurTexture.Sample(g_sampler, input.uv);
    float3 color = (1.0f - bloom.bloomStrength) * color1 + color2 * bloom.bloomStrength;
    color = ToneMapping(color);
    return float4(color, 1.0f);
}