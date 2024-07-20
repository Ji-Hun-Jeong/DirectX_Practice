#include "Header.hlsli"
Texture2D g_heightTexture : register(t0);
cbuffer MeshConstant : register(b2)
{
    matrix world;
    matrix worldIT;
};


PSInput main(VSInput input)
{
    PSInput output;
    
    float4 normal = float4(input.normal, 0.0f);
    normal = mul(normal, worldIT);
    normal = normalize(normal);
    output.normal = normal.xyz;
    
    float4 pos = float4(input.position, 1.0f);
    pos = mul(pos, world);
    if (useHeight)
    {
        // VS에서는 SampleLevel사용
        float height = g_heightTexture.SampleLevel(g_linearSampler, input.uv, 0).r;
        height = 2.0f * height - 1.0f;
        pos += float4(output.normal * height * heightScale, 0.0f);
    }
    output.posWorld = pos;
    pos = mul(pos, viewProj);
    
    output.posProj = pos;
    
    output.uv = input.uv;
    
    output.tangent = mul(float4(input.tangent, 0.0), world).xyz;
    
    return output;
}