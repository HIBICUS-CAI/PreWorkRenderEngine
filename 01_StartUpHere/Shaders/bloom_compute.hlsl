RWTexture2D<unorm float4> BloomTex : register(u0);

groupshared unorm float4 gBloomCache[256 + 2 * 2];

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
        gBloomCache[groupThreadId.x] = BloomTex[int2(x, dispatchThreadId.y)];
    }
    if (groupThreadId.x >= 256 - 2)
    {
        int x = min(dispatchThreadId.x + 2, 1280 - 1);
        gBloomCache[groupThreadId.x + 2 * 2] = BloomTex[int2(x, dispatchThreadId.y)];
    }
    gBloomCache[groupThreadId.x + 2] = BloomTex[min(dispatchThreadId.xy, int2(1280, 720) - 1)];

    GroupMemoryBarrierWithGroupSync();

    float4 blur = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float totalWeight = 0.0f;

    for (int i = -2; i <= 2; ++i)
    {
        int texIndex = groupThreadId.x + 2 + i;
        blur += gBlurWeight[i + 2] * gBloomCache[texIndex];
        totalWeight += gBlurWeight[i + 2];       
    }

    blur /= totalWeight;

    BloomTex[dispatchThreadId.xy] = blur;
}

[numthreads(1, 256, 1)]
void VMain(int3 groupThreadId : SV_GroupThreadID,
    int3 dispatchThreadId : SV_DispatchThreadID)
{
    if (groupThreadId.y < 2)
    {
        int y = max(dispatchThreadId.y - 2, 0);
        gBloomCache[groupThreadId.y] = BloomTex[int2(dispatchThreadId.x, y)];
    }
    if (groupThreadId.y >= 256 - 2)
    {
        int y = min(dispatchThreadId.y + 2, 720 - 1);
        gBloomCache[groupThreadId.y + 2 * 2] = BloomTex[int2(dispatchThreadId.x, y)];
    }
    gBloomCache[groupThreadId.y + 2] = BloomTex[min(dispatchThreadId.xy, int2(1280, 720) - 1)];

    GroupMemoryBarrierWithGroupSync();

    float4 blur = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float totalWeight = 0.0f;

    for (int i = -2; i <= 2; ++i)
    {
        int texIndex = groupThreadId.y + 2 + i;
        blur += gBlurWeight[i + 2] * gBloomCache[texIndex];
        totalWeight += gBlurWeight[i + 2];
    }

    blur /= totalWeight;

    BloomTex[dispatchThreadId.xy] = blur;
}
