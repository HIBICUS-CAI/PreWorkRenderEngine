// TEMP-----------------
static const uint gScreenWidth = 1280;
static const uint gScreenHeight = 720;
static const float3 gSunDirection = float3(0.f, -1.f, -1.f);
static const float4 gSunAmbient = float4(0.3f, 0.3f, 0.3f, 0.3f);
static const float4 gSunColor = float4(0.8f, 0.8f, 0.8f, 0.8f);
// TEMP-----------------

#define NUM_PARTICLES_PER_TILE (1023)
#define PARTICLES_TILE_BUFFER_SIZE (NUM_PARTICLES_PER_TILE + 1)
#define	MAX_PARTICLES_PER_TILE_FOR_SORTING (2 * NUM_PARTICLES_PER_TILE)
#define	MAX_PARTICLES_PER_TILE_FOR_RENDERING (500)

static const uint NUM_COARSE_CULLING_TILES_X = 16;
static const uint NUM_COARSE_CULLING_TILES_Y = 8;
static const uint NUM_COARSE_TILES = NUM_COARSE_CULLING_TILES_X * NUM_COARSE_CULLING_TILES_Y;
static const uint TILE_RES_X = 32;
static const uint TILE_RES_Y = 32;
static const uint NUM_THREADS_X = TILE_RES_X;
static const uint NUM_THREADS_Y = TILE_RES_Y;
static const uint NUM_THREADS_PER_TILE = (NUM_THREADS_X * NUM_THREADS_Y);
static const float THRESHOLD_ALPHA = 0.95f;

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

struct GPUParticlePartA
{
	float4 mColorAndAlpha;
	float2 mViewSpaceVelocityXY;
	float mEmitterNormalDotLight;
	uint mEmitterProperties;
	float mRotation;
	uint mIsSleeping;
	uint mCollisionCount;
	float mPads[1];
};

StructuredBuffer<GPUParticlePartA> gParticleBuffer : register(t0);
StructuredBuffer<float4> gViewSpacePositions : register(t1);
Texture2D<float> gDepthTexture : register(t2);
Buffer<uint> gTiledIndexBuffer : register(t3);
Buffer<uint> gCoarseBufferCounters : register(t4);
Texture2D gParticleTexture : register(t5);

RWBuffer<float4> gOutputBuffer : register(u0);

SamplerState gSamClampLinear : register(s0);

groupshared float4 gParticleTint[MAX_PARTICLES_PER_TILE_FOR_RENDERING];
groupshared uint gParticleEmitterProperties[MAX_PARTICLES_PER_TILE_FOR_RENDERING];
groupshared float gParticleEmitterNdotL[MAX_PARTICLES_PER_TILE_FOR_RENDERING];

groupshared float2 gParticleVelocity[MAX_PARTICLES_PER_TILE_FOR_RENDERING];
groupshared float gParticleStreakLength[MAX_PARTICLES_PER_TILE_FOR_RENDERING];

groupshared float3 gParticlePosition[MAX_PARTICLES_PER_TILE_FOR_RENDERING];
groupshared float gParticleRadius[MAX_PARTICLES_PER_TILE_FOR_RENDERING];

groupshared float gParticleRotation[MAX_PARTICLES_PER_TILE_FOR_RENDERING];

groupshared uint gLdsNumParticles;

float2 CalcEllipsoidRadius(float radius, float2 viewSpaceVelocity)
{
	float minRadius = radius * max(1.f, 0.1f * length(viewSpaceVelocity));
	return float2(radius, minRadius);
}

float GetTextureOffset(uint emitterProperties)
{
	uint index = (emitterProperties & 0x000f0000) >> 16;
	return (float)index / 2.f;
}


bool IsStreakEmitter(uint emitterProperties)
{
	return (emitterProperties >> 24) & 0x01 ? true : false;
}

void InitLDS(uint3 localIdx, uint3 globalIdx)
{
	uint localIdxFlattened = localIdx.x + (localIdx.y * NUM_THREADS_X);
	uint2 screenCoords = globalIdx.xy;
	uint2 cullingTileId =  screenCoords / uint2(TILE_RES_X, TILE_RES_Y);

	uint tileIdxFlattened = cullingTileId.x + cullingTileId.y * gNumTilesX;
	uint tiledStartOffset = PARTICLES_TILE_BUFFER_SIZE * tileIdxFlattened;
	
	if (localIdxFlattened == 0)
	{
		gLdsNumParticles = min(MAX_PARTICLES_PER_TILE_FOR_RENDERING, gTiledIndexBuffer[tiledStartOffset]);
	}

	GroupMemoryBarrierWithGroupSync();
	
	uint numParticlesToCache = gLdsNumParticles;
	for (uint i = localIdxFlattened; i < numParticlesToCache; i += NUM_THREADS_PER_TILE)
	{
		uint globalParticleIndex = gTiledIndexBuffer[tiledStartOffset + 1 + i];
		
		gParticleTint[i] = gParticleBuffer[globalParticleIndex].mColorAndAlpha;
		gParticleEmitterProperties[i] = gParticleBuffer[globalParticleIndex].mEmitterProperties;
		gParticleEmitterNdotL[i] = gParticleBuffer[globalParticleIndex].mEmitterNormalDotLight;

		gParticlePosition[i].xyz = gViewSpacePositions[globalParticleIndex].xyz;
		gParticleRadius[i] = gViewSpacePositions[globalParticleIndex].w;

		gParticleRotation[i] = gParticleBuffer[globalParticleIndex].mRotation;
		
		gParticleVelocity[i] = normalize(gParticleBuffer[globalParticleIndex].mViewSpaceVelocityXY);
		gParticleStreakLength[i] = CalcEllipsoidRadius(gViewSpacePositions[globalParticleIndex].w, gParticleBuffer[globalParticleIndex].mViewSpaceVelocityXY).y;
	}

	GroupMemoryBarrierWithGroupSync();
}

float3 CalcPointOnViewPlane(float3 pointOnPlane, float3 rayDir)
{
	float t = pointOnPlane.z / rayDir.z;
	float3 p = t * rayDir;
	return p;
}

float4 CalcBillboardParticleColor(uint particleIndex, float3 rayDir, float viewSpaceDepth)
{
	uint emitterProperties = gParticleEmitterProperties[particleIndex];
	bool usesStreaks = IsStreakEmitter(emitterProperties);
	float textureOffset = GetTextureOffset(emitterProperties);

	float emitterNdotL = gParticleEmitterNdotL[particleIndex];
	float4 tintAndAlpha = gParticleTint[particleIndex];

	float2 particleVelocity = gParticleVelocity[particleIndex];
	float  particleStreakLength = gParticleStreakLength[particleIndex];

	float3 particleCenter = gParticlePosition[particleIndex];
	float  particleRadius = gParticleRadius[particleIndex];

	float rotationAngle = gParticleRotation[particleIndex];
	

	float3 viewSpacePos = particleCenter;
		
	[branch]
	if (viewSpacePos.z > viewSpaceDepth)
    {
		return (0.f, 0.f, 0.f, 0.f);
    }

	float depthFade = saturate((viewSpaceDepth - viewSpacePos.z) / particleRadius);

	float4 color = (1.f, 1.f, 1.f, 1.f);
	color *= tintAndAlpha;
	color.a *= depthFade;
		
	float3 pointOnPlane = CalcPointOnViewPlane(viewSpacePos, rayDir);
	float2 vecToSurface = (0.f, 0.f);
	float2 rotatedVecToSurface = (0.f, 0.f);
	if (usesStreaks)
	{
		float2 extrusionVector = particleVelocity;
		float2 tangentVector = float2(extrusionVector.y, -extrusionVector.x);

		float2x2 transform = float2x2(tangentVector, extrusionVector);
			
		float2 vecToCentre = pointOnPlane.xy - viewSpacePos.xy;
		vecToCentre = mul(transform, vecToCentre);
		
		float2 radius = float2(particleRadius, particleStreakLength);
		vecToSurface = vecToCentre / radius;
		rotatedVecToSurface = vecToSurface;
	}
	else
	{
		vecToSurface = (pointOnPlane.xy - viewSpacePos.xy) / particleRadius;

		float s, c;
		sincos(rotationAngle, s, c);
		float2x2 rotation = { float2(c, s), float2(-s, c) };
		
		rotatedVecToSurface = mul(vecToSurface, rotation);
	}
	
	float2 rotatedUV = 0.5 * rotatedVecToSurface + 0.5;
	
	[branch]
	if (rotatedUV.x < 0 || rotatedUV.y < 0 || rotatedUV.x > 1 || rotatedUV.y > 1)
	{
		color = (0.f, 0.f, 0.f, 0.f);
	}
	else
	{
		float2 texCoord = rotatedUV;
		texCoord.x *= 0.5f;
		texCoord.x += textureOffset;

		color *= gParticleTexture.SampleLevel(gSamClampLinear, texCoord, 0.f);
		
		float pi = 3.1415926535897932384626433832795f;
		float2 uv = 0.5f * vecToSurface + 0.5f;
		float3 n;
		n.x = -cos(pi * uv.x);
		n.y = -cos(pi * uv.y);
		n.z = sin(pi * length(uv));
		n = normalize(n);
	
        float3 sunDirectionViewSpace = mul((float4)(gSunDirection, 0.f), gView).xyz;
		float ndotl = saturate(dot(sunDirectionViewSpace, n));
		ndotl = lerp(ndotl, emitterNdotL, 0.5);
		float3 lighting = gSunAmbient.rgb + ndotl * gSunColor.rgb;
		color.rgb *= lighting;
	}

	return color;
}

float4 BlendParticlesFrontToBack(float3 viewRay, float viewSpaceDepth)
{
	uint numParticles = gLdsNumParticles;
	
	float4 fcolor = (0.f, 0.f, 0.f, 0.f);
	
	for (uint i = 0; i < numParticles; ++i)
	{	
		float4 color = CalcBillboardParticleColor(i, viewRay, viewSpaceDepth);
		
		fcolor.xyz = (1.f - fcolor.w) * (color.w * color.xyz) + fcolor.xyz;
		fcolor.w = color.w + (1.f - color.w) * fcolor.w;
		
		// If we are close enough to being opaque then let's bail out now
		if (fcolor.w > THRESHOLD_ALPHA)
		{
			// Just force the alpha to 1
			fcolor.w = 1.f;
			break;
		}
	}

	return fcolor;
}

void EvaluateColorAtScreenCoord(uint2 screenSpaceCoord)
{
	float depth = gDepthTexture.Load(uint3(screenSpaceCoord.x, screenSpaceCoord.y, 0)).x;
	float2 screenCoord = (float2)screenSpaceCoord;
	screenCoord += 0.5f;

	float4 viewSpacePos;
	viewSpacePos.x = (screenCoord.x) / (float)gScreenWidth;
	viewSpacePos.y = 1.f - (screenCoord.y / (float)gScreenHeight);
	viewSpacePos.xy = (2.f * viewSpacePos.xy) - 1.f;
	viewSpacePos.z = depth;
	viewSpacePos.w = 1.f;
	
	viewSpacePos = mul(viewSpacePos, gInvProj);
	viewSpacePos.xyz /= viewSpacePos.w;
	float viewSpaceDepth = viewSpacePos.z;

	float3 viewRay = normalize(viewSpacePos.xyz);
	float4 color = BlendParticlesFrontToBack(viewRay, viewSpaceDepth);
	
	if (screenSpaceCoord.x > gScreenWidth)
    {
		screenSpaceCoord.x = gScreenWidth;
    }

	uint pixelLocation = screenSpaceCoord.x + (screenSpaceCoord.y * gScreenWidth);
	
	gOutputBuffer[pixelLocation] = color;
}

[numthreads(NUM_THREADS_X, NUM_THREADS_Y, 1)]
void Main(uint3 localIdx : SV_GroupThreadID, uint3 groupIdx : SV_GroupID, uint3 globalIdx : SV_DispatchThreadID)
{
	InitLDS(localIdx, globalIdx);

	EvaluateColorAtScreenCoord(globalIdx.xy);
}
