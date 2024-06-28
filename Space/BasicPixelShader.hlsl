#include "Header.hlsli"
Texture2D g_texture0 : register(t0);
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
float3 BlinnPhong(float3 normal, float3 toLightVec, float3 toEyeVec, float3 lightStrength)
{
    float3 h = normalize(toLightVec + toEyeVec);
    float hdotn = max(dot(h, normal), 0.0f);
    float3 specular = mat.specular * pow(hdotn, mat.shiness);
    return mat.ambient + (mat.diffuse + specular) * lightStrength;
}
float CalcAttenuation(float dist)
{
    return saturate((light.fallOfEnd - dist) / (light.fallOfEnd - light.fallOfStart));
}

float3 ComputePointLight(PSInput input)
{
    float3 toLightVec = light.lightPos - input.posWorld.xyz;
    float toLightDistance = length(toLightVec);
    if (light.fallOfEnd < toLightDistance)
    {
        return mat.ambient;
    }
    else
    {
        float3 toEyeVec = normalize(eyePos - input.posWorld.xyz);
        float att = CalcAttenuation(toLightDistance);
        float ndotl = max(dot(toLightVec, input.normal), 0.0f);
        float3 finalLightStrength = light.lightStrength * ndotl * att;
        float3 normToLightVec = toLightVec / toLightDistance;
        return BlinnPhong(input.normal, normToLightVec, toEyeVec, finalLightStrength);
    }
}
float4 main(PSInput input) : SV_TARGET
{
    float3 toEye = normalize(eyePos - input.posWorld.xyz);
    float toEyeDist = length(eyePos - input.posWorld.xyz);
    float minMipDist = 300.0f;
    float maxMipDist = 2000.0f;
    float lod = max((toEyeDist - minMipDist), 0.0f) * 10.0f / (maxMipDist - minMipDist);
    float3 color = g_texture0.SampleLevel(g_sampler, input.uv, lod);
    color = isSun ? color * float3(1.5f, 1.5f, 1.5f) : color * ComputePointLight(input);
    if (rim.useRim && !isSun)
    {
        float3 lightVec = normalize(light.lightPos - input.posWorld.xyz);
        float degreeOfLightAndNormal = dot(lightVec, input.normal);
        float degreeOfEyeAndLight = dot(toEye, lightVec);
        if (degreeOfLightAndNormal <= 0.0f)
        {
            float degreeOfEyeAndLight = -dot(toEye, lightVec);
            float degreeOfEyeAndNormal = max(dot(toEye, input.normal), 0.0f);
            color *= pow(1.0f - degreeOfEyeAndNormal, rim.rimPower);
            color *= rim.rimStrength * degreeOfEyeAndLight;
        }
        if (mat.selected)
        {
            float degreeOfEyeAndNormal = max(dot(toEye, input.normal), 0.0f);
            if (degreeOfEyeAndNormal < 0.4f)
            {
                color *= float3(1.0f, 0.0f, 0.0f);
            }
        }
    }
    return float4(color, 1.0f);
}