#include "Header.hlsli"
float4 main(PSInput input) : SV_TARGET
{
    float3 red = float3(1.0f, 0.0f, 0.0f);
    float3 blue = float3(0.0f, 0.0f, 1.0f);
    float3 color = blue * (1 - input.uv.x) + red * (input.uv.x);
	return float4(color, 1.0f);
}