#include "Header.hlsli"
float4 main(PSInput input) : SV_TARGET
{
    float3 red = float3(1.0f, 0.0f, 0.0f);
    float3 green = float3(0.0f, 1.0f, 0.0f);
    float3 blue = float3(0.0f, 0.0f, 1.0f);
    float3 white = float3(1.0f, 1.0f, 1.0f);
    float3 color;
    if (input.uv.y == 0)
        color = red;
    else if (input.uv.y == 0.5f)
        color = green;
    else
        color = blue;
    color = white * (1.0f - input.uv.x) + color * input.uv.x;
    return float4(color, 1.0f);
}