#include "Header.hlsli"

Texture2D g_albedoTexture : register(t0);
Texture2D g_normalTexture : register(t1);
Texture2D g_aoTexture : register(t2);
Texture2D g_emissiveTexture : register(t3);
Texture2D g_metallicTexture : register(t4);
Texture2D g_roughnessTexture : register(t5);

static const float PI = 3.141592f;
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
    float ambientFactor;
    float Dummy;
};

float CalcAttenuation(float dist)
{
    return saturate((light.fallOfEnd - dist) / (light.fallOfEnd - light.fallOfStart));
}

float3 GetNormal(PSInput input)
{
    float3 normal = g_normalTexture.Sample(g_linearSampler, input.uv).xyz;
    normal = 2.0f * normal - 1.0f;
    float3 N = input.normal;
    float3 T = normalize(input.tangent - dot(input.tangent, N) * N);
    float3 B = cross(N, T);
    float3x3 TBN = float3x3(T, B, N);
    normal = normalize(mul(normal, TBN));
    return normal;
}

// 프레넬은 보는각도에 따라서 빛의 세기가 변하는것
float3 SpecularF(float3 F0, float vdoth)
{
    return F0 + (1.0 - F0) * pow(1.0 - vdoth, 5.0);
}

float3 GetDiffuseByIBL(float3 albedo, float3 normal, float3 F)
{
    // normal방향으로 한번만 샘플링 했을 경우 그 지점이 받을 수 있는 빛의 양을 계산해놓은 것이
    // irradiance map
    float3 diffuseByIBL = g_irradianceTexture.Sample(g_linearSampler, normal).rgb;
    return F * diffuseByIBL * albedo;
}

float3 GetSpecularByIBL(float3 albedo, float3 normal, float3 v, float3 F, float roughness)
{
    float3 reflectDir = reflect(-v, normal);
    // IBL에 의한 specular brdf를 구하기 위해 아래의 식 (F0 ~ + ~)이 필요한데 그 식의 ~부분을
    // 미리 Look Up Table로 만들어 둔 것이다.
    // 아래 식의 evBRDF.r과 evBRDF.g
    float3 evBRDF = g_lutTexture.Sample(g_clampSampler, float2(dot(v, normal), 1 - roughness));
    
    // specular는 specular맵에서 완전반사방향으로 한번한 샘플링 하면된다. 정반사이기 때문에
    float3 specularByIBL = g_specularTexture.SampleLevel(g_linearSampler, reflectDir, 5.0f * roughness);
    return specularByIBL * (F * evBRDF.r + evBRDF.g);
}

// 주변광을 구하는 식
// 주변광을 구할 때도 한 픽셀로 난반사, 정반사되어 들어오는 빛의 양을 계산
float3 AmbientLighting(float3 albedo, float3 ao, float3 normal, float3 v
                     , float3 h, float metallic, float roughness)
{
    float3 F0 = 0.04f;
    float3 F = lerp(F0, albedo, metallic);
    float3 diffuse = GetDiffuseByIBL(albedo, normal, 1.0f - F); // 얘가 다른곳에서 오는 난반사
    float3 specular = GetSpecularByIBL(albedo, normal, v, F, roughness); // 얘는 다른곳에서 오는 정반사
    return (diffuse + specular) * ao; // 최종적으로 한 픽셀이 가지는 주변광 따라서 ao도 여기서만 곱하기
}

// Normal Distribution, 우리가 보는 방향이 미세표면의 노말인 표면의 비율 스페큘러에서 하이라이트를 결정
float SpecularD(float ndoth, float roughness)
{
    float minRoughness = max(roughness, 1e-5);
    float alpha = minRoughness * minRoughness;
    float alpha2 = alpha * alpha;
    float denom = ndoth * ndoth * (alpha2 - 1.0) + 1.0;
    return alpha2 / (PI * denom * denom);
}

float G1(float ndotx, float roughness)
{
    float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
    return ndotx / (ndotx * (1.0 - k) + k);
}

// Geometry Funciton, 미세표면에서 반사된 빛이 막히거나 빛이 미세표면에 박혀 못들어오는 상황,
// 기하학적인 형태 표면이 울퉁불퉁함을 결정한다.
float SpecularG(float ndotl, float ndotv, float roughness)
{
    return G1(ndotl, roughness) * G1(ndotv, roughness);
}
float3 GetShadowFactor(float3 posWorld)
{
    float4 lightViewPos = float4(posWorld, 1.0f);
    lightViewPos = mul(lightViewPos, light.lightViewProj);
    lightViewPos.xyz /= lightViewPos.w;
    if (lightViewPos.x < -1 || lightViewPos.x > 1 || lightViewPos.y < -1 || lightViewPos.y > 1)
        return 1.0f;
    
    float2 lightViewScreen = lightViewPos.xy * float2(1.0f, -1.0f);
    lightViewScreen = (lightViewScreen + 1.0f) * 0.5f;
    
    float lightViewDepth = g_lightViewTexture.Sample(g_linearSampler, lightViewScreen);
    
    if (lightViewDepth + 0.0001f < lightViewPos.z)
        return 0.0f;
    else
        return 1.0f;
}
// 실제 광원에서 물체에 맞아 반사되는 빛의 양을 계산
// 이것이 Shading Model에서의 PBR 위의 Image Based Lighting은 할지말지 결정할 수 있는 것
float3 DirectLight(float3 posWorld, float3 albedo, float3 normal, float3 l, float3 v, float3 h, float roughness, float metallic)
{
    float ndotl = max(dot(normal, l), 0.0f);
    float ndoth = max(dot(normal, h), 0.0f);
    float ndotv = max(dot(normal, v), 0.0f);
    float vdoth = max(dot(v, h), 0.0f);
    
    float3 F0 = 0.04f;
    // F0는 반사율.
    // 흡수를 많이하면 albedo값이 적어질 것이고 반사를 전부 하면 albedo값이 되어버릴것이다.
    // 그것이 metallic에 의해 결정되는것이고
    // metallic이 1이되어버리면 아예 반사율이 albedo가 되어버리는 것
    // metallic이 올라갈 수록 색이 어두워지는 이유는 albedo가 0인경우이다.
    F0 = lerp(F0, albedo, metallic);
    float3 F = SpecularF(F0, vdoth);
    float D = SpecularD(ndoth, roughness);
    float G = SpecularG(ndotl, ndotv, roughness);
    // float3 kd = lerp(1.0f - F, 0.0f, metallic);
    
    // 빛의 세기를 난반사와 정반사에 나눠서 주는 것
    // diffuse에는 1-F, specular에는 F
    float3 diffuse = ((1.0f - F) * albedo) / PI;
    float3 specular = (F * D * G) / max(1e-5, 4.0f * ndotl * ndotv);
    float distance = length(l);
    float3 lightStrength = light.lightStrength * CalcAttenuation(distance);
    float3 shadowFactor = GetShadowFactor(posWorld);
    return (diffuse + specular) * lightStrength * shadowFactor;
}

float4 main(PSInput input) : SV_TARGET
{
    if (isLight)
        return float4(1.0f, 1.0f, 0.0f, 1.0f);
    float3 albedo = useAlbedo ? g_albedoTexture.SampleLevel(g_linearSampler, input.uv, 0) : albedoFactor;
    float3 normal = useNormal ? GetNormal(input) : input.normal;
    float3 ao = useAO ? g_aoTexture.SampleLevel(g_linearSampler, input.uv, 0) : 1.0f;
    float metallic = useMetallic ? g_metallicTexture.Sample(g_linearSampler, input.uv).r : metallicFactor;
    float roughness = useRoughness ? g_roughnessTexture.Sample(g_linearSampler, input.uv).r : roughnessFactor;
    
    float3 l = normalize(light.lightPos - input.posWorld.xyz);
    float3 v = normalize(eyePos - input.posWorld.xyz);
    float3 h = normalize(l + v);
    float ndotl = max(0.0f, dot(l, normal));
    
    float3 ambientLight = AmbientLighting(albedo, ao, normal, v, h, metallic, roughness) * ambientFactor;
    float3 directLight = DirectLight(input.posWorld.xyz, albedo, normal, l, v, h, roughness, metallic) * ndotl * light.lightStrength;
    
    float3 emissive = useEmissive ? g_emissiveTexture.SampleLevel(g_linearSampler, input.uv, 0) : emissionFactor;
    float3 color = (ambientLight + directLight) + emissive;
    
    return float4(color, 1.0f);
}