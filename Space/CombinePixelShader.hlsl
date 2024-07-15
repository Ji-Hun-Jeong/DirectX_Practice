#include "Header.hlsli"
Texture2D g_originalTexture : register(t0);
Texture2D g_blurTexture : register(t1);

cbuffer ImageFilterConst : register(b2)
{
    float dx;
    float dy;
    float threshold;
    float strength;
}

float3 ToneMapping(float3 color)
{
    float3 invGamma = float3(1, 1, 1) / gamma;
    color = clamp(exposure * color, 0., 1.);
    color = pow(color, invGamma);
    return color;
}

float4 main(PSInput input) : SV_TARGET
{
    float3 color1 = g_originalTexture.Sample(g_linearSampler, input.uv);
    float3 color2 = g_blurTexture.Sample(g_linearSampler, input.uv);
    float3 color = (1.0f - strength) * color1 + color2 * strength;
    color = ToneMapping(color);
    return float4(color, 1.0f);
}