RWTexture2D<unorm float4> SsaoTex : register(u0);
Texture2D NormalMap : register(t0);
Texture2D DepthMap : register(t1);

groupshared unorm float4 gSsaoCache[256 + 2 * 2];
groupshared float4 gNormalCache[256 + 2 * 2];
groupshared unorm float gDepthCache[256 + 2 * 2];

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
        gSsaoCache[groupThreadId.x] = SsaoTex[int2(x, dispatchThreadId.y)];
        gNormalCache[groupThreadId.x] = NormalMap[int2(x, dispatchThreadId.y)];
        gDepthCache[groupThreadId.x] = DepthMap[int2(x, dispatchThreadId.y)].r;
    }
    if (groupThreadId.x >= 256 - 2)
    {
        int x = min(dispatchThreadId.x + 2, 1280 - 1);
        gSsaoCache[groupThreadId.x + 2 * 2] = SsaoTex[int2(x, dispatchThreadId.y)];
        gNormalCache[groupThreadId.x + 2 * 2] = NormalMap[int2(x, dispatchThreadId.y)];
        gDepthCache[groupThreadId.x + 2 * 2] = DepthMap[int2(x, dispatchThreadId.y)].r;
    }
    gSsaoCache[groupThreadId.x + 2] = SsaoTex[min(dispatchThreadId.xy, int2(1280, 720) - 1)];
    gNormalCache[groupThreadId.x + 2] = NormalMap[min(dispatchThreadId.xy, int2(1280, 720) - 1)];
    gDepthCache[groupThreadId.x + 2] = DepthMap[min(dispatchThreadId.xy, int2(1280, 720) - 1)].r;

    GroupMemoryBarrierWithGroupSync();

    float4 blur = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float4 centerNormal = gNormalCache[groupThreadId.x + 2];
    float centerDepth = gDepthCache[groupThreadId.x + 2];
    float totalWeight = 0.0f;

    for (int i = -2; i <= 2; ++i)
    {
        int texIndex = groupThreadId.x + 2 + i;
        float4 thisNormal = gNormalCache[texIndex];
        float thisDepth = gDepthCache[texIndex];

        if (dot(thisNormal, centerNormal) > 0.8f && abs(thisDepth - centerDepth) < 0.0002f)
        {
            blur += gBlurWeight[i + 2] * gSsaoCache[texIndex];
            totalWeight += gBlurWeight[i + 2];
        }        
    }

    blur /= totalWeight;

    SsaoTex[dispatchThreadId.xy] = blur;
}

[numthreads(1, 256, 1)]
void VMain(int3 groupThreadId : SV_GroupThreadID,
    int3 dispatchThreadId : SV_DispatchThreadID)
{
    if (groupThreadId.y < 2)
    {
        int y = max(dispatchThreadId.y - 2, 0);
        gSsaoCache[groupThreadId.y] = SsaoTex[int2(dispatchThreadId.x, y)];
        gNormalCache[groupThreadId.y] = NormalMap[int2(dispatchThreadId.x, y)];
        gDepthCache[groupThreadId.y] = DepthMap[int2(dispatchThreadId.x, y)].r;
    }
    if (groupThreadId.y >= 256 - 2)
    {
        int y = min(dispatchThreadId.y + 2, 720 - 1);
        gSsaoCache[groupThreadId.y + 2 * 2] = SsaoTex[int2(dispatchThreadId.x, y)];
        gNormalCache[groupThreadId.y + 2 * 2] = NormalMap[int2(dispatchThreadId.x, y)];
        gDepthCache[groupThreadId.y + 2 * 2] = DepthMap[int2(dispatchThreadId.x, y)].r;
    }
    gSsaoCache[groupThreadId.y + 2] = SsaoTex[min(dispatchThreadId.xy, int2(1280, 720) - 1)];
    gNormalCache[groupThreadId.y + 2] = NormalMap[min(dispatchThreadId.xy, int2(1280, 720) - 1)];
    gDepthCache[groupThreadId.y + 2] = DepthMap[min(dispatchThreadId.xy, int2(1280, 720) - 1)].r;

    GroupMemoryBarrierWithGroupSync();

    float4 blur = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float4 centerNormal = gNormalCache[groupThreadId.y + 2];
    float centerDepth = gDepthCache[groupThreadId.y + 2];
    float totalWeight = 0.0f;

    for (int i = -2; i <= 2; ++i)
    {
        int texIndex = groupThreadId.y + 2 + i;
        float4 thisNormal = gNormalCache[texIndex];
        float thisDepth = gDepthCache[texIndex];

        if (dot(thisNormal, centerNormal) > 0.8f && abs(thisDepth - centerDepth) < 0.0002f)
        {
            blur += gBlurWeight[i + 2] * gSsaoCache[texIndex];
            totalWeight += gBlurWeight[i + 2];
        }
    }

    blur /= totalWeight;

    SsaoTex[dispatchThreadId.xy] = blur;
}
