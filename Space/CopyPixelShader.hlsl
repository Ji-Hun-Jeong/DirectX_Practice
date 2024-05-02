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
float4 main(PSInput input) : SV_Target
{
    return g_shaderResourceView.Sample(g_sampler, input.uv);
}