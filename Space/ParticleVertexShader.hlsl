struct GSInput
{
    float4 pos : SV_Position;
    float3 color : COLOR;
    float lifeTime : PSIZE0;
    float radius : PSIZE1;
};
struct Particle
{
    float3 pos;
    float3 color;
    float3 velocity;
    float3 force;
    float lifeTime;
    float radius;
    float density;
    float pressure;
    float size;
};
StructuredBuffer<Particle> g_particle : register(t0);
GSInput main(uint vertexID : SV_VertexID)
{
    Particle p = g_particle[vertexID];
    GSInput output;
    output.pos = float4(p.pos, 1.0f);
    output.color = p.color * saturate(p.lifeTime * 2.0f);
    output.lifeTime = p.lifeTime;
    output.radius = p.radius;
    return output;
}