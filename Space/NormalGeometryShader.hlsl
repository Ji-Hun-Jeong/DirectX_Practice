#include "Header.hlsli"
Texture2D g_normalTexture : register(t0);
cbuffer MeshConstant : register(b2)
{
    matrix world;
    matrix worldIT;
};
float3 GetNormal(GSInput input)
{
    float3 normal = g_normalTexture.SampleLevel(g_linearSampler, input.uv, 0).xyz;
    normal = 2.0f * normal - 1.0f;
    float3 N = input.normal;
    float3 T = normalize(input.tangent - dot(input.tangent, N) * N);
    float3 B = cross(N, T);
    float3x3 TBN = float3x3(T, B, N);
    normal = normalize(mul(normal, TBN));
    return normal;
}
[maxvertexcount(2)]
void main(
	point GSInput input[1], uint primID : SV_PrimitiveID,
	inout LineStream<PSInput> outputStream
)
{
    
	for (uint i = 0; i < 2; i++)
	{
        PSInput output;
        float4 pos = float4(input[0].position, 1.0f);
        float3 normal = useNormal ? GetNormal(input[0]) : input[0].normal;
        pos.xyz += normal * i * normalSize;
        output.posWorld = pos;
        pos = mul(pos, viewProj);
        output.posProj = pos;
        
        output.normal = normal;
        output.uv = input[0].uv;
        output.uv.x = i;
        output.tangent = input[0].tangent;
        outputStream.Append(output);
    }
}