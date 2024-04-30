#include "Header.hlsli"
cbuffer PixelConstant : register(b0)
{
    float3 eyePos;
    int isSun;
    Light light;
    Material mat;
};
float4 main(PSInput input) : SV_TARGET
{
    float3 color;
    color = float3(0.0f, 0.0f, 1.0f);
	return float4(color, 1.0f);
}