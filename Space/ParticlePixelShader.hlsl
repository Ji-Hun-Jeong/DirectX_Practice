struct PSInput
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float3 color : COLOR;
};
float smootherstep(float x, float edge0 = 0.0f, float edge1 = 1.0f)
{
  // Scale, and clamp x to 0..1 range
    x = clamp((x - edge0) / (edge1 - edge0), 0, 1);

    return x * x * x * (3 * x * (2 * x - 5) + 10.0f);
}
float4 main(PSInput input) : SV_TARGET
{
    float2 center = 0.5f;
    float dist = length(center- input.uv) * 2;
    float3 color = input.color;
    float scale = smootherstep(1 - dist);
    color *= scale;
    return float4(color, 1.0f);
}