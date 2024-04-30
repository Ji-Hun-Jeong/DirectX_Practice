#include "Header.hlsli"
TextureCube g_textureCubeMap : register(t0);
SamplerState g_sampler : register(s0);
cbuffer PixelConstant : register(b0)
{
    float3 eyePos;
    int isSun;
    Light light;
    Material mat;
};
float4 main(PSInput input) : SV_TARGET
{
    return g_textureCubeMap.Sample(g_sampler, input.posWorld.xyz);
}