RWTexture2D<unorm float4> SsaoTex : register(u0);

[numthreads(16, 16, 1)]
void main(int3 dispatchThreadId : SV_DispatchThreadID)
{
    SsaoTex[dispatchThreadId.xy] = 1.0f - SsaoTex[dispatchThreadId.xy];
}