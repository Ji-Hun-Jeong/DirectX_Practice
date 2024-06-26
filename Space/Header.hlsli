struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};
struct GSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};
struct PSInput
{
    float4 posProj : SV_Position;
    float4 posWorld : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
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