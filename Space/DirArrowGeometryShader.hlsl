#include "Header.hlsli"
cbuffer MeshConstant : register(b1)
{
    matrix model;
    matrix invTranspose;
};
cbuffer CommonConstant : register(b2)
{
    int useHeight = false;
    float heightScale = 1.0f;
    int useNormal = false;
    float normalSize = 1.0f;
};
[maxvertexcount(2)]
void main(
	point GSInput input[1], uint primID : SV_PrimitiveID,
	inout LineStream<PSInput> outputStream
)
{
    PSInput output;
    float4 originPos = float4(input[0].position, 1.0f);
    originPos = mul(originPos, model);
    originPos = mul(originPos, viewProj);
    output.posProj = originPos;
    output.uv = float2(0.0f, float(primID) / 2.0f);
    output.normal = input[0].normal;
    output.posWorld = originPos;
    output.tangent = input[0].tangent;
    outputStream.Append(output);
    
    // float4 arrowPos = float4(input[0].position, 1.0f);
    // float3 arr[3] = { viewDir, upDir, rightDir };
    // arrowPos.xyz += arr[primID] * arrowSize;
    // arrowPos = mul(arrowPos, model);
    // originPos = mul(originPos, view);
    // originPos = mul(originPos, projection);
    // output.posProj = arrowPos;
    output.uv = float2(1.0f, float(primID) / 2.0f);
    outputStream.Append(output);
}