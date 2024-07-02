#include "Header.hlsli"
Texture2D g_heightTexture : register(t0);
SamplerState g_sampler : register(s0);
cbuffer Constant : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
    matrix invTranspose;
    int useHeight;
    float heightScale;
    float dummy[2];
};
PSInput main(VSInput input)
{
    PSInput output;
    
    float4 normal = float4(input.normal, 0.0f);
    normal = mul(normal, invTranspose);
    normal = normalize(normal);
    output.normal = normal.xyz;
    
    float4 pos = float4(input.position, 1.0f);
    if (useHeight)
    {
        float height = g_heightTexture.SampleLevel(g_sampler, input.uv, 0).r;
        height = 2.0f * height - 1.0f;
        pos += float4(output.normal * height * heightScale, 0.0f);
    }
    pos = mul(pos, model);
    output.posWorld = pos;
    pos = mul(pos, view);
    pos = mul(pos, projection);
    output.posProj = pos;
    
    
    output.uv = input.uv;
    
    output.tangent = mul(float4(input.tangent, 0.0), model).xyz;
    
    return output;
}