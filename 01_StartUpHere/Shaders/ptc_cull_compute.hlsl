// TEMP---------------
#define gScreenWidth (1280)
#define gScreenHeight (720)
// TEMP--------------

#define NUM_PARTICLES_PER_TILE (1023)
#define PARTICLES_TILE_BUFFER_SIZE (NUM_PARTICLES_PER_TILE + 1)
#define	MAX_PARTICLES_PER_TILE_FOR_SORTING (2 * NUM_PARTICLES_PER_TILE)

#define NUM_COARSE_CULLING_TILES_X (16)
#define NUM_COARSE_CULLING_TILES_Y (8)
#define NUM_COARSE_TILES (NUM_COARSE_CULLING_TILES_X * NUM_COARSE_CULLING_TILES_Y)
#define TILE_RES_X (32)
#define TILE_RES_Y (32)

cbuffer CameraStatus : register(b0)
{
    matrix gView;
    matrix gInvView;
    matrix gProj;
    matrix gInvProj;
    matrix gViewProj;
    float3 gEyePosition;
    float gCameraPad[1];
}

cbuffer TilingConstantBuffer : register(b1)
{
    uint gNumTilesX;
    uint gNumTilesY;
    uint gNumCoarseCullingTilesX;
    uint gNumCoarseCullingTilesY;
    uint gNumCullingTilesPerCoarseTileX;
    uint gNumCullingTilesPerCoarseTileY;
    uint gTilingPads[2];
};

cbuffer ActiveListCount : register(b2)
{
    uint gNumActiveParticles;
    uint3 gActiveListCountPad;
};

StructuredBuffer<float4> gViewSpacePositions : register(t0);
StructuredBuffer<float> gMaxRadiusBuffer : register(t1);
StructuredBuffer<float2> gAliveIndexBuffer : register(t2);
Texture2D<float> gDepthTexture : register(t3);
Buffer<uint> gCoarseBuffer : register(t4);
Buffer<uint> gCoarseBufferCounters : register(t5);

RWBuffer<uint> gTiledIndexBuffer : register(u0);

groupshared uint gLdsZMax;
groupshared uint gLdsParticleIdx[MAX_PARTICLES_PER_TILE_FOR_SORTING];
groupshared float gLdsParticleDistances[MAX_PARTICLES_PER_TILE_FOR_SORTING];
groupshared uint gLdsNumParticles;

float3 ConvertProjToView(float4 p)
{
    p = mul(p, gInvProj);
    p /= p.w;
    
    return p.xyz;
}

float3 CreatePlaneEquation(float3 b, float3 c)
{
    return normalize(cross(b, c));
}

float ConvertProjDepthToView(float z)
{
    z = 1.f / (z * gInvProj._34 + gInvProj._44);
    return z;
}


void CalculateMinMaxDepthInLds(uint3 globalIdx)
{
    float opaqueDepth = gDepthTexture.Load(uint3(globalIdx.x, globalIdx.y, 0)).x;
    float opaqueViewPosZ = ConvertProjDepthToView(opaqueDepth);
    uint opaqueZ = asuint(opaqueViewPosZ);

    if(opaqueDepth != 0.f)
    {
        InterlockedMax(gLdsZMax, opaqueZ);
    }	
}

void CalcFrustumPlanes(in uint2 groupIdx, out float3 frustumEqn[4])
{
    int pxm = TILE_RES_X * groupIdx.x;
    int pym = TILE_RES_Y * groupIdx.y;
    int pxp = TILE_RES_X * (groupIdx.x + 1);
    int pyp = TILE_RES_Y * (groupIdx.y + 1);

    float3 frustum[4];
    frustum[0] = ConvertProjToView(float4(pxm / (float)gScreenWidth * 2.f - 1.f, (gScreenHeight - pym) / (float)gScreenHeight * 2.f - 1.f, 1.f, 1.f));
    frustum[1] = ConvertProjToView(float4(pxp / (float)gScreenWidth * 2.f - 1.f, (gScreenHeight - pym) / (float)gScreenHeight * 2.f - 1.f, 1.f, 1.f));
    frustum[2] = ConvertProjToView(float4(pxp / (float)gScreenWidth * 2.f - 1.f, (gScreenHeight - pyp) / (float)gScreenHeight * 2.f - 1.f, 1.f, 1.f));
    frustum[3] = ConvertProjToView(float4(pxm / (float)gScreenWidth * 2.f - 1.f, (gScreenHeight - pyp) / (float)gScreenHeight * 2.f - 1.f, 1.f, 1.f));

    for (uint i = 0; i < 4; i++)
    {
        frustumEqn[i] = CreatePlaneEquation(frustum[i], frustum[(i + 1) & 3]);
    }
}

float GetSignedDistanceFromPlane(float3 p, float3 eqn)
{
    return dot(eqn, p);
}

void BitonicSort(in uint localIdxFlattened)
{
    uint numParticles = gLdsNumParticles;
    uint numParticlesPowerOfTwo = 1;
    while (numParticlesPowerOfTwo < numParticles)
    {
        numParticlesPowerOfTwo <<= 1;
    }

    GroupMemoryBarrierWithGroupSync();

    for(uint nMergeSize = 2; nMergeSize <= numParticlesPowerOfTwo; nMergeSize *= 2)
    {
        for(uint nMergeSubSize = nMergeSize >> 1; nMergeSubSize > 0; nMergeSubSize = nMergeSubSize >> 1) 
        {		
            uint tmp_index = localIdxFlattened;
            uint index_low = tmp_index & (nMergeSubSize - 1);
            uint index_high = 2 * (tmp_index - index_low);
            uint index = index_high + index_low;
            uint nSwapElem = (nMergeSubSize == nMergeSize >> 1) ? index_high + (2 * nMergeSubSize - 1) - index_low : index_high + nMergeSubSize + index_low;

            if (nSwapElem < numParticles && index < numParticles)
            {
                if (gLdsParticleDistances[index] > gLdsParticleDistances[nSwapElem])
                { 
                    uint uTemp = gLdsParticleIdx[index];
                    float vTemp = gLdsParticleDistances[index];

                    gLdsParticleIdx[index] = gLdsParticleIdx[nSwapElem];
                    gLdsParticleDistances[index] = gLdsParticleDistances[nSwapElem];
                    gLdsParticleIdx[nSwapElem] = uTemp;
                    gLdsParticleDistances[nSwapElem] = vTemp;
                }
            }
            GroupMemoryBarrierWithGroupSync();
        }
    }
}

uint GetNumParticlesInCoarseTile(uint tile)
{
    return gCoarseBufferCounters[tile];
}

uint getParticleIndexFromCoarseBuffer(uint binIndex, uint listIndex)
{
    uint offset = binIndex * gNumActiveParticles;
    return gCoarseBuffer[offset + listIndex];
}

void AddParticleToVisibleList(uint index, float distance)
{
    uint dstIdx = 0;
    InterlockedAdd(gLdsNumParticles, 1, dstIdx);
                    
    if (dstIdx < MAX_PARTICLES_PER_TILE_FOR_SORTING)
    {
        gLdsParticleIdx[dstIdx] = index;
        gLdsParticleDistances[dstIdx] = distance;
    }
}

[numthreads(TILE_RES_X, TILE_RES_Y, 1)]
void Main(uint3 localIdx : SV_GroupThreadID, uint3 groupIdx : SV_GroupID, uint3 globalIdx : SV_DispatchThreadID)
{
    uint localIdxFlattened = localIdx.x + (localIdx.y * TILE_RES_X);
    uint i = 0;

    if(localIdxFlattened == 0)
    {
        gLdsNumParticles = 0;
        gLdsZMax = 0;
    } 
    
    GroupMemoryBarrierWithGroupSync();

    CalculateMinMaxDepthInLds(globalIdx);

    GroupMemoryBarrierWithGroupSync();

    float maxZ = asfloat(gLdsZMax);
    float3 frustumEqn[4];
    CalcFrustumPlanes(groupIdx.xy, frustumEqn);

    uint tileX = groupIdx.x / gNumCullingTilesPerCoarseTileX;
    uint tileY = groupIdx.y / gNumCullingTilesPerCoarseTileY;
    uint coarseTileIdx = tileX + tileY * gNumCoarseCullingTilesX;
    uint uNumParticles = GetNumParticlesInCoarseTile(coarseTileIdx);

    for (i = localIdxFlattened; i < uNumParticles; i += TILE_RES_X * TILE_RES_Y)
    {
        uint index = getParticleIndexFromCoarseBuffer(coarseTileIdx, i);
        float r = gMaxRadiusBuffer[index];
        float4 vsPosition = gViewSpacePositions[index];
        float3 center = vsPosition.xyz;
        
        if (center.z - maxZ < r)
        {
            if((GetSignedDistanceFromPlane(center, frustumEqn[0]) < r) &&
                (GetSignedDistanceFromPlane(center, frustumEqn[1]) < r) &&
                (GetSignedDistanceFromPlane(center, frustumEqn[2]) < r) &&
                (GetSignedDistanceFromPlane(center, frustumEqn[3]) < r))
            {
                AddParticleToVisibleList(index, vsPosition.z);
            }
        }
    }
    
    GroupMemoryBarrierWithGroupSync();
    
    BitonicSort(localIdxFlattened);
    
    if(localIdxFlattened == 0)
    {
        gLdsNumParticles = min(gLdsNumParticles, NUM_PARTICLES_PER_TILE);
    }

    GroupMemoryBarrierWithGroupSync();
    
    uint tileIdxFlattened = groupIdx.x + groupIdx.y * gNumTilesX;
    uint tiledBufferStartOffset = PARTICLES_TILE_BUFFER_SIZE * tileIdxFlattened;
    uint numLDSParticleToCache = gLdsNumParticles;
    
    for (i = localIdxFlattened; i < numLDSParticleToCache; i += TILE_RES_X*TILE_RES_Y)
    {
        gTiledIndexBuffer[tiledBufferStartOffset + 1 + i] = gLdsParticleIdx[i];
    }

    if (localIdxFlattened == 0)
    {
        gTiledIndexBuffer[tiledBufferStartOffset] = numLDSParticleToCache;
    }
}
