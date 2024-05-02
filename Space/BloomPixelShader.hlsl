#include "Header.hlsli"
Texture2D g_shaderResourceView : register(t0);
SamplerState g_sampler : register(s0);
cbuffer PixelConstant : register(b0)
{
    float3 eyePos;
    int isSun;
    Light light;
    Material mat;
    float threshold;
    float dx;
    float dy;
    float bloomLightStrength;
};
float4 main(PSInput input) : SV_TARGET
{
    float3 color;
    color = g_shaderResourceView.Sample(g_sampler, input.uv);
    float luminance = color.r * 0.2126 + color.g * 0.7152 + color.b * 0.0722;
    if (luminance < threshold)
        color = float3(0.0f, 0.0f, 0.0f);
    return float4(color, 1.0f);
}