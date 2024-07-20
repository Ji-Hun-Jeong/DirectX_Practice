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
    matrix lightViewProj;
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
    matrix invProj;
    
    float3 eyePos;
    float strengthIBL;

    Light light;
    
    float exposure;
    float gamma;
    float2 dummy;
}

cbuffer CommonConstant : register(b1)
{
    int useHeight = false;
    float heightScale = 1.0f;
    int useNormal = false;
    float normalSize = 1.0f;
    
    int mode;
    float depthStrength = 1.0f;
    float fogStrength = 0.0f;
    float dummy1;
};

TextureCube g_specularTexture : register(t10);
TextureCube g_irradianceTexture : register(t11);
Texture2D g_lutTexture : register(t12);
Texture2D g_lightViewTexture : register(t13);

SamplerState g_linearSampler : register(s0);
SamplerState g_clampSampler : register(s1);