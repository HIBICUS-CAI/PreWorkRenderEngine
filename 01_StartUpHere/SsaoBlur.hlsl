RWTexture2D<unorm float4> SsaoTex : register(u0);

groupshared unorm float4 gCache[256 + 2 * 2];

static const float gBlurWeight[5] = 
{
    0.0545f, 0.2442f, 0.4026f, 0.2442f, 0.0545f
};

[numthreads(256, 1, 1)]
void HMain(int3 groupThreadId : SV_GroupThreadID,
    int3 dispatchThreadId : SV_DispatchThreadID)
{
    if (groupThreadId.x < 2)
    {
        int x = max(dispatchThreadId.x - 2, 0);
        gCache[groupThreadId.x] = SsaoTex[int2(x, dispatchThreadId.y)];
    }
    if (groupThreadId.x >= 256 - 2)
    {
        int x = min(dispatchThreadId.x + 2, 1280 - 1);
        gCache[groupThreadId.x + 2 * 2] = SsaoTex[int2(x, dispatchThreadId.y)];
    }
    gCache[groupThreadId.x + 2] = SsaoTex[min(dispatchThreadId.xy, int2(1280, 720) - 1)];

    GroupMemoryBarrierWithGroupSync();

    // SsaoTex[dispatchThreadId.xy] = 1.0f - SsaoTex[dispatchThreadId.xy];
    // SsaoTex[dispatchThreadId.xy] = 1.0f - gCache[groupThreadId.x + 2];

    float4 blur = float4(0.0f, 0.0f, 0.0f, 0.0f);

    for (int i = -2; i <= 2; ++i)
    {
        int texIndex = groupThreadId.x + 2 + i;
        blur += gBlurWeight[i + 2] * gCache[texIndex];
    }

    SsaoTex[dispatchThreadId.xy] = blur;
}

[numthreads(1, 256, 1)]
void VMain(int3 groupThreadId : SV_GroupThreadID,
    int3 dispatchThreadId : SV_DispatchThreadID)
{
    if (groupThreadId.y < 2)
    {
        int y = max(dispatchThreadId.y - 2, 0);
        gCache[groupThreadId.y] = SsaoTex[int2(dispatchThreadId.x, y)];
    }
    if (groupThreadId.y >= 256 - 2)
    {
        int y = min(dispatchThreadId.y + 2, 720 - 1);
        gCache[groupThreadId.y + 2 * 2] = SsaoTex[int2(dispatchThreadId.x, y)];
    }
    gCache[groupThreadId.y + 2] = SsaoTex[min(dispatchThreadId.xy, int2(1280, 720) - 1)];

    GroupMemoryBarrierWithGroupSync();

    // SsaoTex[dispatchThreadId.xy] = 1.0f - SsaoTex[dispatchThreadId.xy];
    // SsaoTex[dispatchThreadId.xy] = 1.0f - gCache[groupThreadId.x + 2];

    float4 blur = float4(0.0f, 0.0f, 0.0f, 0.0f);

    for (int i = -2; i <= 2; ++i)
    {
        int texIndex = groupThreadId.y + 2 + i;
        blur += gBlurWeight[i + 2] * gCache[texIndex];
    }

    SsaoTex[dispatchThreadId.xy] = blur;
}