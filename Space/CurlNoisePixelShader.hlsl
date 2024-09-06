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
float CubicSpline(const float q)
{
    float coeff = 3.0f / (2.0f * 3.141592f);

    if (q < 1.0f)
        return coeff * (2.0f / 3.0f - q * q + 0.5f * q * q * q);
    else if (q < 2.0f)
        return coeff * pow(2.0f - q, 3.0f) / 6.0f;
    else // q >= 2.0f
        return 0.0f;
}
float4 main(PSInput input) : SV_TARGET
{
    float dist = length(float2(0.5, 0.5) - input.uv) * 2.0f;
    
    float scale = smootherstep(1.0f - dist);
    return float4(input.color.rgb * scale, 1);
}