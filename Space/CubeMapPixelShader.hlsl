#include "Header.hlsli"
TextureCube g_specularCubeMap : register(t0);
TextureCube g_irradianceCubeMap : register(t1);
SamplerState g_sampler : register(s0);
cbuffer PixelConstant : register(b0)
{
    float3 eyePos;
    int isSun;
    Light light;
    Material mat;
    Bloom bloom;
    Rim rim;
    int useAlbedo;
    int useNormal;
    int useAO;
    int useRoughness;
    
    int useMetallic;
    float exposure;
    float gamma;
    float dummy;
};
float3 ToneMapping(float3 color)
{
    float invGamma = float3(1.0f, 1.0f, 1.0f) / gamma;
    float3 result = pow(clamp(color * exposure, 0.0f, 1.0f), invGamma);
    return result;
}
float4 main(PSInput input) : SV_TARGET
{
    float3 color = g_specularCubeMap.Sample(g_sampler, input.posWorld.xyz).rgb;
    return float4(ToneMapping(color), 1.0f);

}