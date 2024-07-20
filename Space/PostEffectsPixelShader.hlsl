#include "Header.hlsli"
Texture2D g_originalTexture : register(t20);
Texture2D g_depthTexture : register(t21);

float4 ToViewSpace(float2 uv)
{
    float4 posView;
    posView.xy = uv * 2.0f - 1.0f;
    posView.y *= -1;
    posView.z = g_depthTexture.Sample(g_linearSampler, uv).r;
    posView.w = 1.0f;
    posView = mul(posView, invProj);
    posView.xyz /= posView.w;
    return posView;
}
float4 main(PSInput input) : SV_TARGET
{
    float3 depth = ToViewSpace(input.uv).z * depthStrength * 0.1f;
    float3 color = 1.0f;
    if (mode == 1)
        color = depth;
    else
    {
        float4 posView = ToViewSpace(input.uv);
        float dist = length(posView.xyz); // 눈의 위치가 원점인 좌표계

        // TODO: Fog
        float3 fogColor = float3(1.0f, 1.0f, 1.0f);
        float fogMin = 1.0f;
        float fogMax = 10.0f;
        float distFog = saturate((dist - fogMin) / (fogMax - fogMin));
        float3 fogFactor = exp(-distFog * fogStrength);
        color = g_originalTexture.Sample(g_linearSampler, input.uv).rgb;
        color = lerp(fogColor, color, fogFactor);
    }
    return float4(color, 1.0f);
}