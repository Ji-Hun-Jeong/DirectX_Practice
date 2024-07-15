#include "Header.hlsli"
Texture2D g_shaderResourceView : register(t0);
cbuffer ImageFilterConst : register(b2)
{
    float dx;
    float dy;
    float threshold;
    float strength;
}
float4 main(PSInput input) : SV_Target
{
    return g_shaderResourceView.Sample(g_linearSampler, input.uv);
}