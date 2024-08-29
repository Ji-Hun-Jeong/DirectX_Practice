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
    return saturate((dist - light.fallOfStart) / (light.fallOfEnd - light.fallOfStart));
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

// �������� ���°����� ���� ���� ���Ⱑ ���ϴ°�
float3 SpecularF(float3 F0, float vdoth)
{
    return F0 + (1.0 - F0) * pow(1.0 - vdoth, 5.0);
}

float3 GetDiffuseByIBL(float3 albedo, float3 normal, float3 F)
{
    // normal�������� �ѹ��� ���ø� ���� ��� �� ������ ���� �� �ִ� ���� ���� ����س��� ����
    // irradiance map
    float3 diffuseByIBL = g_irradianceTexture.Sample(g_linearSampler, normal).rgb;
    return F * diffuseByIBL * albedo;
}

float3 GetSpecularByIBL(float3 albedo, float3 normal, float3 v, float3 F, float roughness)
{
    float3 reflectDir = reflect(-v, normal);
    // IBL�� ���� specular brdf�� ���ϱ� ���� �Ʒ��� �� (F0 ~ + ~)�� �ʿ��ѵ� �� ���� ~�κ���
    // �̸� Look Up Table�� ����� �� ���̴�.
    // �Ʒ� ���� evBRDF.r�� evBRDF.g
    float3 evBRDF = g_lutTexture.Sample(g_clampSampler, float2(dot(v, normal), 1 - roughness));
    
    // specular�� specular�ʿ��� �����ݻ�������� �ѹ��� ���ø� �ϸ�ȴ�. ���ݻ��̱� ������
    float3 specularByIBL = g_specularTexture.SampleLevel(g_linearSampler, reflectDir, 5.0f * roughness);
    return specularByIBL * (F * evBRDF.r + evBRDF.g);
}

// �ֺ����� ���ϴ� ��
// �ֺ����� ���� ���� �� �ȼ��� ���ݻ�, ���ݻ�Ǿ� ������ ���� ���� ���
float3 AmbientLighting(float3 albedo, float3 ao, float3 normal, float3 v
                     , float3 h, float metallic, float roughness)
{
    float3 F0 = 0.04f;
    float3 F = lerp(F0, albedo, metallic);
    float3 diffuse = GetDiffuseByIBL(albedo, normal, 1.0f - F); // �갡 �ٸ������� ���� ���ݻ�
    float3 specular = GetSpecularByIBL(albedo, normal, v, F, roughness); // ��� �ٸ������� ���� ���ݻ�
    return (diffuse + specular) * ao; // ���������� �� �ȼ��� ������ �ֺ��� ���� ao�� ���⼭�� ���ϱ�
}

// Normal Distribution, �츮�� ���� ������ �̼�ǥ���� �븻�� ǥ���� ���� ����ŧ������ ���̶���Ʈ�� ����
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

// Geometry Funciton, �̼�ǥ�鿡�� �ݻ�� ���� �����ų� ���� �̼�ǥ�鿡 ���� �������� ��Ȳ,
// ���������� ���� ǥ���� ������������ �����Ѵ�.
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
// ���� �������� ��ü�� �¾� �ݻ�Ǵ� ���� ���� ���
// �̰��� Shading Model������ PBR ���� Image Based Lighting�� �������� ������ �� �ִ� ��
float3 DirectLight(float3 posWorld, float3 albedo, float3 normal, float3 l, float3 v, float3 h, float roughness, float metallic)
{
    float ndotl = max(dot(normal, l), 0.0f);
    float ndoth = max(dot(normal, h), 0.0f);
    float ndotv = max(dot(normal, v), 0.0f);
    float vdoth = max(dot(v, h), 0.0f);
    
    float3 F0 = 0.04f;
    // F0�� �ݻ���.
    // ����� �����ϸ� albedo���� ������ ���̰� �ݻ縦 ���� �ϸ� albedo���� �Ǿ�������̴�.
    // �װ��� metallic�� ���� �����Ǵ°��̰�
    // metallic�� 1�̵Ǿ������ �ƿ� �ݻ����� albedo�� �Ǿ������ ��
    // metallic�� �ö� ���� ���� ��ο����� ������ albedo�� 0�ΰ���̴�.
    F0 = lerp(F0, albedo, metallic);
    float3 F = SpecularF(F0, vdoth);
    
    float alpha = roughness * roughness;
    float alphaPrime = saturate(alpha + light.radius / (2 * length(l)));
    float sphereNormalization = alpha * alpha / (alphaPrime * alphaPrime);
    float D = SpecularD(ndoth, roughness) * sphereNormalization;
    
    float G = SpecularG(ndotl, ndotv, roughness);
    // float3 kd = lerp(1.0f - F, 0.0f, metallic);
    
    // ���� ���⸦ ���ݻ�� ���ݻ翡 ������ �ִ� ��
    // diffuse���� 1-F, specular���� F
    // Metallic�� ���ؼ��� �ݻ籤�� ���Ⱑ �����ǰ� Roughness�� ���ؼ��� �ݻ籤�� ���̶���Ʈ�� �����ȴ�.
    float3 diffuse = ((1.0f - F) * albedo) / PI;
    float3 specular = (F * D * G) / max(1e-5, 4.0f * ndotl * ndotv);
    float distance = length(l);
    float spotTheta = max(0.0f, dot(normalize(posWorld - light.lightPos), light.lightDir));
    float3 lightStrength = max(0.0f, light.lightStrength * pow(CalcAttenuation(distance)
    * spotTheta, light.spotFactor));
    float3 shadowFactor = GetShadowFactor(posWorld);
    return (diffuse + specular) * lightStrength * shadowFactor * ndotl;
}

float3 GetLightPos(float3 center, float3 posWorld, float3 normal)
{
    float3 eyeToPixel = normalize(posWorld - eyePos);
    float3 reflectVec = normalize(reflect(eyeToPixel, normal));
    float3 pixelToLight = center - posWorld;
    float3 centerToReflect = dot(pixelToLight, reflectVec) * reflectVec - pixelToLight;
    float centerToReflectDist = length(centerToReflect);
    if (centerToReflectDist > light.radius)
        centerToReflectDist = light.radius;
    centerToReflect = normalize(centerToReflect) * centerToReflectDist;
    return center + centerToReflect;
}

float4 main(PSInput input) : SV_TARGET
{
    // if (isLight)
    //     return float4(1.0f, 1.0f, 0.0f, 1.0f);
    float3 albedo = useAlbedo ? g_albedoTexture.SampleLevel(g_linearSampler, input.uv, 0) : albedoFactor;
    float3 normal = useNormal ? GetNormal(input) : input.normal;
    float3 ao = useAO ? g_aoTexture.SampleLevel(g_linearSampler, input.uv, 0) : 1.0f;
    float metallic = useMetallic ? g_metallicTexture.Sample(g_linearSampler, input.uv).r : metallicFactor;
    float roughness = useRoughness ? g_roughnessTexture.Sample(g_linearSampler, input.uv).r : roughnessFactor;
    
    float3 lightPos = GetLightPos(light.lightPos, input.posWorld.xyz, normal);
    float3 l = normalize(lightPos - input.posWorld.xyz);
    float3 v = normalize(eyePos - input.posWorld.xyz);
    float3 h = normalize(l + v);
    float ndotl = max(0.0f, dot(l, normal));
    
    float3 ambientLight = AmbientLighting(albedo, ao, normal, v, h, metallic, roughness) * ambientFactor;
    float3 directLight = DirectLight(input.posWorld.xyz, albedo, normal, l, v, h, roughness, metallic);
    
    float3 emissive = useEmissive ? g_emissiveTexture.SampleLevel(g_linearSampler, input.uv, 0) : emissionFactor;
    float3 color = (ambientLight + directLight) + emissive;
    
    return float4(color, 1.0f);
}