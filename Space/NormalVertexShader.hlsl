#include "Header.hlsli"
Texture2D g_normalTexture : register(t0);
SamplerState g_sampler : register(s0);
cbuffer NormalConstant : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
    matrix invTranspose;
    float normalSize;
    bool useNormal;
    float2 dummy;
};
float3 GetNormal(VSInput input)
{
    float3 normal = g_normalTexture.SampleLevel(g_sampler, input.uv,0).xyz;
    normal = 2.0f * normal - 1.0f;
    float3 N = input.normal;
    float3 T = normalize(input.tangent - dot(input.tangent, N) * N);
    float3 B = cross(N, T);
    float3x3 TBN = float3x3(T, B, N);
    normal = normalize(mul(normal, TBN));
    return normal;
}

GSInput main(VSInput input) 
{
    GSInput output;
    float4 pos = float4(input.position, 1.0f);
    float4 normal = float4(input.normal, 0.0f);
    pos = mul(pos, model);
   
    normal = mul(normal, invTranspose);
    normal = normalize(normal);
    normal = useNormal ? float4(GetNormal(input), 0.0f) : normal;
    
    output.position = pos.xyz;
    output.normal = normal;
     
    output.uv.xy = 0.0f;
    return output;
}