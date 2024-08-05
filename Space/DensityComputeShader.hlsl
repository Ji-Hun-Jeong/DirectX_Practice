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
RWTexture2D<float> g_rtv : register(u0);
RWStructuredBuffer<Particle> g_sb : register(u1);
cbuffer StandardParticle : register(b0)
{
    int i;
    float3 Dummy;
}
float CubicSpline(float q)
{
    if (q <= 0)
        q = 1e-5f;
    q *= 2.0f;
    float value = 3.0f / (2.0f * 3.141592f);
    if (q < 1.0f)
        value *= 2.0f / 3.0f - q * q + q * q * q / 2.0f;
    else if (q < 2.0f)
        value *= (2.0f - q) * (2.0f - q) * (2.0f - q) / 6.0f;
    else
        value *= 0.0f;

    return value;
}
[numthreads(1024, 1, 1)]
void main(uint3 gtId : SV_GroupID, uint3 dtId : SV_DispatchThreadID)
{
    if (g_sb[dtId.x].lifeTime < 0.0f)
        return;

    const float dist = length(g_sb[i].pos - g_sb[dtId.x].pos);
    const float radius = g_sb[i].radius;

    if (dist >= radius)
        return;
    uint2 index = 0;
    g_rtv[dtId.xy] += CubicSpline(dist / radius);
}