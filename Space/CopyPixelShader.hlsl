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
};
float4 main(PSInput input) : SV_Target
{
    return g_shaderResourceView.Sample(g_sampler, input.uv);
}