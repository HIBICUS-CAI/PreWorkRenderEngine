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
    float mPad;
};

RWStructuredBuffer<GPUParticlePartA> g_ParticleBufferA : register(u0);
RWStructuredBuffer<GPUParticlePartB> g_ParticleBufferB : register(u1);

[numthreads(256, 1, 1)]
void Main(uint3 id : SV_DispatchThreadID)
{
	g_ParticleBufferA[id.x] = (GPUParticlePartA)0;
	g_ParticleBufferB[id.x] = (GPUParticlePartB)0;
}