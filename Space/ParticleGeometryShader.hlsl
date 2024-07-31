struct GSInput
{
    float4 pos : SV_Position;
    float3 color : COLOR;
    float lifeTime : PSIZE0;
    float radius : PSIZE1;
};
struct PSInput
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float3 color : COLOR;
};

[maxvertexcount(4)]
void main(
	point GSInput input[1],
	inout TriangleStream<PSInput> outputStream
)
{
    if(input[0].lifeTime<=0.0f)
        return;
    float2 offset[4] = { float2(-1, -1), float2(-1, 1), float2(1, -1), float2(1, 1) };
    for (uint i = 0; i < 4; i++)
	{
        PSInput output;
        output.pos = float4(input[0].pos.xy + offset[i] * input[0].radius, 1.0f, 1.0f);
        output.uv = offset[i];
        output.uv = (output.uv * float2(1.0f, -1.0f) + 1.0f) * 0.5f;
        output.color = input[0].color;
        outputStream.Append(output);
    }
}