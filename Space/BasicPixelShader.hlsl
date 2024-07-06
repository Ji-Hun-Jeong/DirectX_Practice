#include "Header.hlsli"
TextureCube g_specularTexture : register(t0);
TextureCube g_irradianceTexture : register(t1);
Texture2D g_albedoTexture : register(t2);
Texture2D g_normalTexture : register(t3);
Texture2D g_aoTexture : register(t4);
Texture2D g_metallicTexture : register(t5);
Texture2D g_roughnessTexture : register(t6);
Texture2D g_lutTexture : register(t7);
SamplerState g_sampler : register(s0);
SamplerState g_clampSampler : register(s1);
static const float pi = 3.141592f;
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
    float Metallic;
};

float CalcAttenuation(float dist)
{
    return saturate((light.fallOfEnd - dist) / (light.fallOfEnd - light.fallOfStart));
}

float3 GetNormal(PSInput input)
{
    float3 normal = g_normalTexture.Sample(g_sampler, input.uv).xyz;
    normal = 2.0f * normal - 1.0f;
    float3 N = input.normal;
    float3 T = normalize(input.tangent - dot(input.tangent, N) * N);
    float3 B = cross(N, T);
    float3x3 TBN = float3x3(T, B, N);
    normal = normalize(mul(normal, TBN));
    return normal;
}

float3 SpecularF(float3 F0, float vdoth)
{
    float _2Pow = (-5.55473f * vdoth - 6.98316f) * vdoth;
    return F0 + ((1 - F0) * pow(2, _2Pow));
}

float3 GetDiffuseByIBL(float3 albedo, float3 normal, float3 F)
{
    float3 diffuseByIBL = g_irradianceTexture.Sample(g_sampler, normal).rgb;
    return F * diffuseByIBL * albedo;
}

float3 GetSpecularByIBL(float3 albedo, float3 normal, float3 v, float3 F, float roughness)
{
    float3 reflectDir = reflect(-v, normal);
    float3 evBRDF = g_lutTexture.Sample(g_clampSampler, float2(dot(v, normal), 1 - roughness));
    float3 specularByIBL = g_specularTexture.SampleLevel(g_sampler, reflectDir, 2.0f * roughness);
    return specularByIBL * (F * evBRDF.r + evBRDF.g);
}

float3 AmbientLighting(float3 albedo, float3 ao, float3 normal, float3 v
                     , float3 h, float metallic, float roughness)
{
    // 주변광을 구하는 식
    // 주변광을 구할 때도 한 픽셀로 들어오는 주변광과 직접광을 전부 계산
    float3 F0 = 0.04f;
    float3 F = lerp(F0, albedo, metallic);
    float3 diffuse = GetDiffuseByIBL(albedo, normal, 1.0f - F); // 얘가 다른곳에서 오는 주변광
    float3 specular = GetSpecularByIBL(albedo, normal, v, F, roughness); // 얘는 다른곳에서 오는 직접광
    return (diffuse + specular) * ao ; // 최종적으로 한 픽셀이 가지는 주변광 따라서 ao도 여기서만 곱하기
}

float SpecularD(float ndoth,float roughness)
{
    float aa = roughness * roughness * roughness * roughness;
    return aa / (pi * (ndoth * ndoth * (aa - 1) + 1) * (ndoth * ndoth * (aa - 1) + 1));
}

float G1(float ndotx, float roughness)
{
    float k = ((roughness + 1) * (roughness + 1)) / 8.0f;
    float value = ndotx / ((ndotx * (1 - k)) + k);
    return value;
}

float SpecularG(float ndotl, float ndotv, float roughness)
{
    return G1(ndotl, roughness) * G1(ndotv, roughness);
}

float3 DirectLight(float3 albedo, float3 normal, float3 l, float3 v, float3 h, float roughness, float metallic)
{
    float ndotl = max(dot(normal, l), 0.0f);
    float ndoth = max(dot(normal, h), 0.0f);
    float ndotv = max(dot(normal, v), 0.0f);
    float vdoth = max(dot(v, h), 0.0f);
    
    float3 F0 = 0.04f;
    // F0는 반사율.
    // 흡수를 많이하면 albedo값이 적어질 것이고 반사를 전부 하면 albedo값이 되어버릴것이다.
    // 그것이 metallic에 의해 결정되는것이고
    F0 = lerp(F0, albedo, metallic);
    float3 F = SpecularF(F0, vdoth);
    float D = SpecularD(ndoth, roughness);
    float G = SpecularG(ndotl, ndotv, roughness);
    // float3 kd = lerp(1.0f - F, 0.0f, metallic);
    
    float3 diffuse = ((1.0f - F) * albedo) / pi;
    float3 specular = (F * D * G) / max(1e-5, 4.0f * ndotl * ndotv);

    return diffuse + specular;
}

float4 main(PSInput input) : SV_TARGET
{    
    float3 albedo = useAlbedo ? g_albedoTexture.SampleLevel(g_sampler, input.uv, 0) : 1.0f;
    float3 normal = useNormal ? GetNormal(input) : input.normal;
    float3 ao = useAO ? g_aoTexture.SampleLevel(g_sampler, input.uv, 0) : 1.0f;
    float metallic = useMetallic ? g_metallicTexture.Sample(g_sampler, input.uv).r : Metallic;
    float roughness = useRoughness ? g_roughnessTexture.Sample(g_sampler, input.uv).r : 0.0f;
    
    float3 l = normalize(light.lightPos - input.posWorld.xyz);
    float3 v = normalize(eyePos - input.posWorld.xyz);
    float3 h = normalize(l + v);
    float ndotl = max(0.0f, dot(l, normal));
    
    float3 ambientLight = AmbientLighting(albedo, ao, normal, v, h, metallic, roughness);
    float3 directLight = DirectLight(albedo, normal, l, v, h, roughness, metallic) * ndotl * light.lightStrength;
    float3 color = ambientLight + directLight; // ambientLight; //+ 
    return float4(color, 1.0f);
}