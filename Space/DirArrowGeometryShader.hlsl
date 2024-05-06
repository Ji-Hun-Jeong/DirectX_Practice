#include "Header.hlsli"
cbuffer Constant : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
    matrix invTranspose;
    float3 viewDir;
    float arrowSize = 1.0f;
    float3 upDir;
    float dummy1 = 0.0f;
    float3 rightDir;
    float dummy2 = 0.0f;
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
    originPos = mul(originPos, view);
    originPos = mul(originPos, projection);
    output.posProj = originPos;
    output.uv.xy = float2(0.0f, 0.0f);
    output.normal = input[0].normal;
    output.posWorld = originPos;
    outputStream.Append(output);
    
    float4 arrowPos = float4(input[0].position, 1.0f);
    float3 arr[3] = { viewDir, upDir, rightDir };
    arrowPos.xyz += arr[primID] * arrowSize;
    arrowPos = mul(arrowPos, model);
    arrowPos = mul(arrowPos, view);
    arrowPos = mul(arrowPos, projection);
    output.posProj = arrowPos;
    output.uv.xy = float2(1.0f, 0.0f);
    outputStream.Append(output);
}