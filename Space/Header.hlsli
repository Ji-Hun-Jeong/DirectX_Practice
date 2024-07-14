struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
};
struct GSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
};
struct PSInput
{
    float4 posProj : SV_Position;
    float4 posWorld : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
};

struct Light	// 32
{
    float3 lightPos;
    float fallOfStart;
    float3 lightStrength;
    float fallOfEnd;
};

struct Material	// 48
{
    float3 ambient;
    float dummy1;
    float3 diffuse;
    int selected;
    float3 specular;
    float shiness;
};

struct Bloom
{
    float threshold;
    float dx;
    float dy;
    float bloomStrength;
};

struct Rim
{
    float rimStrength;
    float rimPower;
    float dummy;
    int useRim;
};

cbuffer GlobalConstData : register(b0)
{
    matrix view;
    matrix proj;
    matrix viewProj;
    float3 eyePos;
    float strengthIBL;

    Light light;
    
    float exposure;
    float gamma;
    float2 dummy;
}

TextureCube g_specularTexture : register(t10);
TextureCube g_irradianceTexture : register(t11);
Texture2D g_lutTexture : register(t12);

SamplerState g_linearSampler : register(s0);
SamplerState g_clampSampler : register(s1);