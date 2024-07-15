#include "Header.hlsli"
Texture2D g_albedoTexture : register(t0);
Texture2D g_normalTexture : register(t1);
Texture2D g_aoTexture : register(t2);
Texture2D g_emissiveTexture : register(t3);
Texture2D g_metallicTexture : register(t4);
Texture2D g_roughnessTexture : register(t5);
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

float3 BlinnPhong(float3 normal, float3 toLightVec, float3 toEyeVec, float3 lightStrength)
{
    float3 h = normalize(toLightVec + toEyeVec);
    float hdotn = max(dot(h, normal), 0.0f);
    return 1.0f;
    //float3 specular = mat.specular * pow(hdotn, mat.shiness);
    //return mat.ambient + (mat.diffuse + specular) * lightStrength;
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
        return 1.0f;
        //return mat.ambient;
    }
    else
    {
        float3 toEyeVec = normalize(eyePos - input.posWorld.xyz);
        float att = max(CalcAttenuation(toLightDistance), 0.0f);
        float ndotl = max(dot(toLightVec, input.normal), 0.0f);
        float3 finalLightStrength = light.lightStrength * ndotl * att;
        float3 normToLightVec = toLightVec / toLightDistance;
        return BlinnPhong(input.normal, normToLightVec, toEyeVec, finalLightStrength);
    }
}

float GetLOD(PSInput input)
{
    float toEyeDist = length(eyePos - input.posWorld.xyz);
    float minMipDist = 100.0f;
    float maxMipDist = 3000.0f;
    float lod = max((toEyeDist - minMipDist), 0.0f) * 10.0f / (maxMipDist - minMipDist);
    return lod;
}
float4 main(PSInput input) : SV_TARGET
{
    PSInput Input = input;
    float3 toEye = normalize(eyePos - input.posWorld.xyz);
    float3 color = float3(1.0f, 1.0f, 1.0f);
    
    if (useAlbedo)
    {
        color = g_albedoTexture.SampleLevel(g_linearSampler, Input.uv, 0).rgb;
    }
    
    if (useNormal)
    {
        float3 normal = g_normalTexture.SampleLevel(g_linearSampler, input.uv, 0);
        normal = 2.0f * normal - 1.0f;
        float3 N = input.normal;
        float3 T = input.tangent;
        float3 B = normalize(cross(N, T));
        float3x3 TBN = float3x3(T, B, N);
        normal = normalize(mul(normal, TBN));
        Input.normal = normal;
    }
    
    // color = isSun ? color * float3(1.5f, 1.5f, 1.5f) : color * ComputePointLight(Input);
    
    if (useAO)
    {
        float3 ao = g_aoTexture.SampleLevel(g_linearSampler, input.uv, 0);
        color *= ao;
    }
    
    // if (rim.useRim && !isSun)
    // {
    //     float3 lightVec = normalize(light.lightPos - Input.posWorld.xyz);
    //     float degreeOfLightAndNormal = dot(lightVec, Input.normal);
    //     float degreeOfEyeAndLight = dot(toEye, lightVec);
    //     if (degreeOfLightAndNormal <= 0.0f)
    //     {
    //         float degreeOfEyeAndLight = -dot(toEye, lightVec);
    //         float degreeOfEyeAndNormal = max(dot(toEye, Input.normal), 0.0f);
    //         color *= pow(1.0f - degreeOfEyeAndNormal, rim.rimPower);
    //         color *= rim.rimStrength * degreeOfEyeAndLight;
    //     }
    //     if (mat.selected)
    //     {
    //         float degreeOfEyeAndNormal = max(dot(toEye, Input.normal), 0.0f);
    //         return float4(1.0f, 0.0f, 1.0f, 1.0f);
    //         if (degreeOfEyeAndNormal < 0.4f)
    //         {
    //             color *= float3(1.0f, 0.0f, 0.0f);
    //         }
    //     }
    // }
    // if (mat.selected)
    // {
    //     // 얜 원래 노말 쓰기
    //     float degreeOfEyeAndNormal = max(dot(toEye, input.normal), 0.0f);
    //     if (degreeOfEyeAndNormal < 0.1f)
    //     {
    //         color *= float3(1.0f, 0.0f, 0.0f);
    //     }
    // }
    float3 specular = g_specularTexture.SampleLevel(g_linearSampler, Input.normal, 0);
    float3 diffuse = g_irradianceTexture.SampleLevel(g_linearSampler, Input.normal, 0);
    return float4(color * specular * diffuse, 1.0f);
}