#include "Header.hlsli"
PSInput main(VSInput input)
{
    PSInput output;
    output.posWorld = float4(input.position, 1.0f);
    output.posProj = output.posWorld;
    output.normal = input.normal;
    output.uv = input.uv;
    return output;
}