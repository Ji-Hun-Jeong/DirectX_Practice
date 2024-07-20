#include "Header.hlsli"
Texture2D g_originalTexture : register(t20);
Texture2D g_depthTexture : register(t21);

float4 UVToView(float2 uv)
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
int IntersectRayCollision(in float3 o, in float3 d, in float3 center, in float radius, out float t1, out float t2)
{
    float3 p = o - center;
    float pdotv = dot(p, d);
    float p2 = dot(p, p);
    float r2 = radius * radius;
    float m = pdotv * pdotv - (p2 - r2);
    if (m < 0.0 || pdotv > 0)
    {
        t1 = 0;
        t2 = 0;
        return 0;
    }
    else
    {
        m = sqrt(m);
        t1 = -pdotv - m;
        t2 = -pdotv + m;
        return 1;
    }
}
float3 GetHaloFactor(float3 pixelViewPos)
{
    float3 eyeViewPos = 0.0f;
    float3 lightViewPos = mul(float4(light.lightPos, 1.0f), view).xyz;
    
    float3 p = eyeViewPos - lightViewPos;
    float3 v = normalize(pixelViewPos - eyeViewPos);
    float t1 = 0;
    float t2 = 0;
    if (IntersectRayCollision(eyeViewPos, v, lightViewPos, light.radius, t1, t2))
    {
        float p2 = dot(p, p);
        float pdotv = dot(p, v);
        float r2 = light.radius * light.radius;
        float invr2 = 1.0 / r2;
        float haloEmission = (1 - p2 * invr2) * (t2 - t1)
                          - pdotv * invr2 * (t2 * t2 - t1 * t1)
                          - 1.0 / (3.0 * r2) * (t2 * t2 * t2 - t1 * t1 * t1);
            
        haloEmission /= (4 * light.radius / 3.0);

        return haloEmission;
    }
    else
        return 0;
}

float4 main(PSInput input) : SV_TARGET
{
    float3 depth = UVToView(input.uv).z * depthStrength * 0.1f;
    float3 color = g_originalTexture.Sample(g_linearSampler, input.uv).rgb;
    if (mode == 1)
        color = depth;
    else
    {
        float4 posView = UVToView(input.uv);
        float dist = length(posView.xyz); // 눈의 위치가 원점인 좌표계
        float3 haloColor = float3(0.96, 0.94, 0.82);
        color += GetHaloFactor(posView.xyz) * haloColor * light.haloFactor;
        
        float3 fogColor = float3(1.0f, 1.0f, 1.0f);
        float fogMin = 1.0f;
        float fogMax = 10.0f;
        float distFog = saturate((dist - fogMin) / (fogMax - fogMin));
        float3 fogFactor = exp(-distFog * fogStrength);
 
        color = lerp(fogColor, color, fogFactor);
        
    }
    return float4(color, 1.0f);
}