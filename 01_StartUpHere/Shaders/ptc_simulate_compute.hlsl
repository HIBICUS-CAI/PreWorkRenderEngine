// TEMP-----------------------------
static const uint gScreenWidth = 1280;
static const uint gScreenHeight = 720;
static const float gDeltaTime = 0.016f;
static const float gCollisionThickness = 4.f;
static const float3 gSunDirection = float3(0.f, -1.f, -1.f);
// TEMP-----------------------------

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

struct GPUParticlePartB
{
	float3 mWorldPosition;
	float mMass;
	float3 mWorldSpaceVelocity;
	float mLifeSpan;
	float mDistanceToEye;
	float mAge;
	float mStartSize;
	float mEndSize;
    float4 mStartColor;
    float4 mEndColor;
    float3 mAcceleration;
    float mPads[1];
};

struct EMITTER_INFO
{
    float3 mWorldPosition;
    float mPads[1];
};

cbuffer CameraStatus : register(b0)
{
    matrix gView;
    matrix gInvView;
    matrix gProj;
    matrix gInvProj;
    matrix gViewProj;
    float3 gEyePosition;
    float gPad[1];
}

RWStructuredBuffer<GPUParticlePartA> gParticleBufferA : register(u0);
RWStructuredBuffer<GPUParticlePartB> gParticleBufferB : register(u1);
AppendStructuredBuffer<uint> gDeadListToAddTo : register(u2);
RWStructuredBuffer<float2> gAliveIndexBuffer : register(u3);
RWStructuredBuffer<float4> gViewSpacePositions : register(u4);
RWStructuredBuffer<float> gMaxRadiusBuffer : register(u5);

Texture2D gDepthBuffer : register(t0);
StructuredBuffer<EMITTER_INFO> gEmitterInfo : register(t1);

float3 CalcViewSpacePositionFromDepth(float2 normalizedScreenPosition, int2 texelOffset)
{
	normalizedScreenPosition.x += (float)texelOffset.x / (float)gScreenWidth;
	normalizedScreenPosition.y += (float)texelOffset.y / (float)gScreenHeight;

    float2 uv;
	uv.x = (0.5f + normalizedScreenPosition.x * 0.5f) * (float)gScreenWidth; 
	uv.y = (1.f - (0.5f + normalizedScreenPosition.y * 0.5f)) * (float)gScreenHeight; 

	float depth = gDepthBuffer.Load(uint3(uv.x, uv.y, 0)).x;
	
	float4 viewSpacePosOfDepthBuffer;
	viewSpacePosOfDepthBuffer.xy = normalizedScreenPosition.xy;
	viewSpacePosOfDepthBuffer.z = depth;
	viewSpacePosOfDepthBuffer.w = 1;

	viewSpacePosOfDepthBuffer = mul(viewSpacePosOfDepthBuffer, gInvProj);
	viewSpacePosOfDepthBuffer.xyz /= viewSpacePosOfDepthBuffer.w;

	return viewSpacePosOfDepthBuffer.xyz;
}

uint GetEmitterIndex(uint emitterProperties)
{
	return emitterProperties & 0xffff;
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

float2 CalcEllipsoidRadius(float radius, float2 viewSpaceVelocity)
{
	float minRadius = radius * max(1.f, 0.1f * length(viewSpaceVelocity));
	return float2(radius, minRadius);
}

[numthreads(256, 1, 1)]
void Main(uint3 id : SV_DispatchThreadID)
{
	GPUParticlePartA pa = gParticleBufferA[id.x];
	GPUParticlePartB pb = gParticleBufferB[id.x];
	
	if (pb.mAge > 0.f)
	{
		uint emitterIndex = GetEmitterIndex(pa.mEmitterProperties);
		bool streaks = IsStreakEmitter(pa.mEmitterProperties);

		pb.mAge -= gDeltaTime;
		pa.mRotation += 0.24f * gDeltaTime;

		float3 vNewPosition = pb.mWorldPosition;

		if (pa.mIsSleeping == 0)
		{
			pb.mWorldSpaceVelocity += pb.mMass * pb.mAcceleration * gDeltaTime;
			vNewPosition += pb.mWorldSpaceVelocity * gDeltaTime;
		}
	
		float fScaledLife = 1.f - saturate(pb.mAge / pb.mLifeSpan);
		float radius = lerp(pb.mStartSize, pb.mEndSize, fScaledLife);
		bool killParticle = false;

		float3 viewSpaceParticlePosition =  mul(float4( vNewPosition, 1), gView).xyz;
		float4 screenSpaceParticlePosition =  mul(float4(vNewPosition, 1), gViewProj);
		screenSpaceParticlePosition.xyz /= screenSpaceParticlePosition.w;

		if (pa.mIsSleeping == 0 && screenSpaceParticlePosition.x > -1 &&
            screenSpaceParticlePosition.x < 1 &&
            screenSpaceParticlePosition.y > -1 && screenSpaceParticlePosition.y < 1)
		{
			float3 viewSpacePosOfDepthBuffer = CalcViewSpacePositionFromDepth(screenSpaceParticlePosition.xy, int2(0, 0));

			if ((viewSpaceParticlePosition.z > viewSpacePosOfDepthBuffer.z) && (viewSpaceParticlePosition.z < viewSpacePosOfDepthBuffer.z + gCollisionThickness))
			{
				float3 p0 = viewSpacePosOfDepthBuffer;
				float3 p1 = CalcViewSpacePositionFromDepth(screenSpaceParticlePosition.xy, int2(1, 0));
				float3 p2 = CalcViewSpacePositionFromDepth(screenSpaceParticlePosition.xy, int2(0, 1));

				float3 viewSpaceNormal = normalize(cross(p2 - p0, p1 - p0));
				float3 surfaceNormal = normalize(mul(-viewSpaceNormal, (float3x4)gInvView).xyz);
				float3 newVelocity = reflect(pb.mWorldSpaceVelocity, surfaceNormal);
				pb.mWorldSpaceVelocity = 0.3f * newVelocity;
				vNewPosition = pb.mWorldPosition + (pb.mWorldSpaceVelocity * gDeltaTime);

				++pa.mCollisionCount;
			}
		}
	
		if (pa.mCollisionCount > 10 && length(pb.mWorldSpaceVelocity) < 0.01f)
		{
			pa.mIsSleeping = 1;
		}

		if (vNewPosition.y < -10.f)
		{
			killParticle = true;
		}

		pb.mWorldPosition = vNewPosition;

		float3 vec = vNewPosition - gEyePosition;
		pb.mDistanceToEye = length(vec);

		float alpha = lerp(1.f, 0.f, saturate(fScaledLife - 0.8f) / 0.2f);

		float4 color0 = pb.mStartColor;
		float4 color1 = pb.mEndColor;
	
		pa.mColorAndAlpha = lerp(color0, color1, saturate(5*fScaledLife));
		
		float2 emitterNormal = normalize(vNewPosition.xz - gEmitterInfo[emitterIndex].mWorldPosition.xz);
		float emitterNdotL = saturate(dot(gSunDirection.xz, emitterNormal) + 0.5f);
		float2 vsVelocity = mul(float4(pb.mWorldSpaceVelocity, 0.f), gView).xy;
		
		pa.mViewSpaceVelocityXY = vsVelocity;
		pa.mEmitterNormalDotLight = emitterNdotL;

		float4 viewSpacePositionAndRadius;
		viewSpacePositionAndRadius.xyz = mul(float4(vNewPosition, 1.f), gView).xyz;
		viewSpacePositionAndRadius.w = radius;

		gViewSpacePositions[id.x] = viewSpacePositionAndRadius;

		if (streaks)
		{
			float2 r2 = CalcEllipsoidRadius(radius, pa.mViewSpaceVelocityXY);
			gMaxRadiusBuffer[id.x] = max(r2.x, r2.y);
		}
		else
		{
			gMaxRadiusBuffer[id.x] = 1.41f * radius;
		}

		if ( pb.mAge <= 0.f || killParticle )
		{
			pb.mAge = -1.f;
			gDeadListToAddTo.Append(id.x);
		}
		else
		{
			uint index = gAliveIndexBuffer.IncrementCounter();
			gAliveIndexBuffer[index] = float2(pb.mDistanceToEye, (float)id.x);
		}
	}

	gParticleBufferA[id.x] = pa;
	gParticleBufferB[id.x] = pb;
}

