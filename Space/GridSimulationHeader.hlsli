cbuffer GridConst : register(b0)
{
    float4 densityColor;
    float2 velocity;
    uint mouseX;
    uint mouseY;
    float dt;
    float viscosity;
    float2 dummy;
}

SamplerState g_linearSampler : register(s0);
SamplerState g_clampSampler : register(s1);