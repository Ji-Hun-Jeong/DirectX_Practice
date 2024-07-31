RWTexture2D<float4> g_rtv : register(u0);

[numthreads(32, 32, 1)]
void main(uint3 gtId : SV_GroupID, uint3 dtId : SV_DispatchThreadID)
{    
    float dissipate = 0.1f;
    float4 color = float4(max(0.0f, g_rtv[dtId.xy].rgb - dissipate), 1.0f);
    g_rtv[dtId.xy] = color;
}