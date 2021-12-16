// TEMP---------------
static const uint gScreenWidth = 1280;
static const uint gScreenHeight = 720;
// TEMP---------------

#define COARSE_CULLING_THREADS 256

static const uint NUM_COARSE_CULLING_TILES_X = 16;
static const uint NUM_COARSE_CULLING_TILES_Y = 8;
static const uint NUM_COARSE_TILES = NUM_COARSE_CULLING_TILES_X * NUM_COARSE_CULLING_TILES_Y;
static const uint TILE_RES_X = 32;
static const uint TILE_RES_Y = 32;

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

RWBuffer<uint> gCoarseTiledIndexBuffer : register(u0);
RWBuffer<uint> gCoarseTiledIndexBufferCounters : register( u1 );

groupshared float3 gFrustumData[NUM_COARSE_CULLING_TILES_X][NUM_COARSE_CULLING_TILES_Y][4];

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

float GetSignedDistanceFromPlane(float3 p, float3 eqn)
{
    return dot(eqn, p);
}

void InitLDS(uint localIdx)
{
    if (localIdx < NUM_COARSE_TILES)
    {
        uint coarseTileWidth = gNumCullingTilesPerCoarseTileX * TILE_RES_X;
        uint coarseTileHeight = gNumCullingTilesPerCoarseTileY * TILE_RES_Y;
        uint tileX = localIdx % NUM_COARSE_CULLING_TILES_X;
        uint tileY = localIdx / NUM_COARSE_CULLING_TILES_X;
            
        int pxm = tileX * coarseTileWidth;
        int pym = tileY * coarseTileHeight;
        int pxp = (tileX + 1) * coarseTileWidth;
        int pyp = (tileY + 1) * coarseTileHeight;

        // Generate the four side planes
        float3 frustum[4];
        frustum[0] = ConvertProjToView(float4(pxm / (float)gScreenWidth * 2.f - 1.f, (gScreenHeight - pym) / (float)gScreenHeight * 2.f - 1.f, 1.f, 1.f));
        frustum[1] = ConvertProjToView(float4(pxp / (float)gScreenWidth * 2.f - 1.f, (gScreenHeight - pym) / (float)gScreenHeight * 2.f - 1.f, 1.f, 1.f));
        frustum[2] = ConvertProjToView(float4(pxp / (float)gScreenWidth * 2.f - 1.f, (gScreenHeight - pyp) / (float)gScreenHeight * 2.f - 1.f, 1.f, 1.f));
        frustum[3] = ConvertProjToView(float4(pxm / (float)gScreenWidth * 2.f - 1.f, (gScreenHeight - pyp) / (float)gScreenHeight * 2.f - 1.f, 1.f, 1.f));
                    
        for(int i = 0; i < 4; ++i)
        {
            gFrustumData[tileX][tileY][i] = CreatePlaneEquation(frustum[i], frustum[(i + 1) & 3]);
        }
    }
}

void AddToBuffer(uint bufferIndex, uint particleGlobalIndex)
{
    uint dstIdx = 0;
    InterlockedAdd(gCoarseTiledIndexBufferCounters[bufferIndex], 1, dstIdx);

    dstIdx += bufferIndex * gNumActiveParticles;
    gCoarseTiledIndexBuffer[dstIdx] = particleGlobalIndex;
}

[numthreads(COARSE_CULLING_THREADS, 1, 1)]
void Main( uint3 localIdx : SV_GroupThreadID, uint3 groupIdx : SV_GroupID, uint3 globalIdx : SV_DispatchThreadID )
{
    InitLDS(localIdx.x);

    if (globalIdx.x < NUM_COARSE_TILES)
    {
        gCoarseTiledIndexBufferCounters[globalIdx.x] = 0;
    }

    GroupMemoryBarrierWithGroupSync();

    if (globalIdx.x < gNumActiveParticles)
    {
        uint index = (uint)gAliveIndexBuffer[globalIdx.x].y;
            
        float4 vsPosition = gViewSpacePositions[index];
        float3 center = vsPosition.xyz;

        float r = gMaxRadiusBuffer[index];
        
        if (-center.z < r)
        {
            for (int tileX = 0; tileX < NUM_COARSE_CULLING_TILES_X; ++tileX)
            {
                for (int tileY = 0; tileY < NUM_COARSE_CULLING_TILES_Y; ++tileY)
                {
                    // Do frustum plane tests
                    if ((GetSignedDistanceFromPlane(center, gFrustumData[tileX][tileY][0]) < r) &&
                         (GetSignedDistanceFromPlane(center, gFrustumData[tileX][tileY][1]) < r) &&
                         (GetSignedDistanceFromPlane(center, gFrustumData[tileX][tileY][2]) < r) &&
                         (GetSignedDistanceFromPlane(center, gFrustumData[tileX][tileY][3]) < r))
                    {
                        AddToBuffer(tileY * NUM_COARSE_CULLING_TILES_X + tileX, index);
                    }
                }
            }
        }
    }
}
