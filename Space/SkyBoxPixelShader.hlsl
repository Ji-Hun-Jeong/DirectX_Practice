#include "Header.hlsli"
cbuffer MaterialConstant : register(b2)
{
    float3 albedoFactor;
    float metallicFactor;
    float3 emissionFactor;
    float roughnessFactor;

    int useAlbedo;
    int useAO;
    int useEmissive;
    int useRoughness;
    
    int useMetallic;
    int isLight;
    float Dummy[2];
};
float4 main(PSInput input) : SV_TARGET
{
    float3 color = g_specularTexture.Sample(g_linearSampler, input.posWorld.xyz).rgb;
    return float4(color, 1.0f);

}