#include "NoiseHeader.hlsli"
struct GSInput
{
    float4 pos : SV_Position;
    float3 color : COLOR;
};

StructuredBuffer<Particle> g_particle : register(t0);

GSInput main(uint vertexID : SV_VertexID)
{
    Particle p = g_particle[vertexID];
    GSInput output;
    output.pos = float4(p.pos, 1.0f);
    output.color = p.color;
    return output;
}