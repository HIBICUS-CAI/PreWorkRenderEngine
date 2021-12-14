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

cbuffer EmitterConstantBuffer : register(b0)
{
    uint gEmitterIndex;
    float gEmitNumPerSecond;
    uint gNumToEmit;
    float gAccumulation;
    float3 gPosition;
    float3 gVelocity;
    float3 gPosVariance;
    float3 gAcceleration;
    float gVelVariance;
    float gParticleMass;
    float gLifeSpan;
    float gOffsetStartSize;
    float gOffsetEndSize;
	uint gTextureID;
    uint gStreakFlg;
    uint gMiscFlg;
    float4 gOffsetStartColor;
    float4 gOffsetEndColor;
};

cbuffer DeadListCount : register(b1)
{
	uint gNumDeadParticles;
	uint3 gDeadListCountPad;
};

Texture2D gRandomTexture : register(t0);

RWStructuredBuffer<GPUParticlePartA> gParticleBufferA : register(u0);
RWStructuredBuffer<GPUParticlePartB> gParticleBufferB : register(u1);
ConsumeStructuredBuffer<uint> gDeadListToAllocFrom : register(u2);

SamplerState gSamWrapLinear : register(s0);

uint WriteEmitterProperties(uint emitterIndex, uint textureIndex, bool isStreakEmitter)
{
	uint properties = emitterIndex & 0xffff;
	properties |= textureIndex << 16;
	if ( isStreakEmitter ) { properties |= 1 << 24; }

	return properties;
}

[numthreads(1024, 1, 1)]
void Main(uint3 id : SV_DispatchThreadID)
{
	if (id.x < gNumDeadParticles && id.x < gNumToEmit)
	{
		GPUParticlePartA pa = (GPUParticlePartA)0;
		GPUParticlePartB pb = (GPUParticlePartB)0;
		
        static float timer = 0.f;
        timer += 0.016f;
		float2 uv0 = float2(id.x / 1024.0, timer);
		float3 randomValues0 = gRandomTexture.SampleLevel(gSamWrapLinear, uv0, 0.f).xyz;
		float2 uv1 = float2((id.x + 1) / 1024.0, timer);
		float3 randomValues1 = gRandomTexture.SampleLevel(gSamWrapLinear, uv1, 0.f).xyz;


		pa.mEmitterProperties = WriteEmitterProperties(gEmitterIndex, gTextureID, gStreakFlg ? true : false);
		pa.mRotation = 0.f;
		pa.mIsSleeping = 0;
		pa.mCollisionCount = 0;

        float velocityMagnitude = length(gVelocity);
        pb.mWorldPosition = gPosition + (randomValues0 * gPosVariance);
		pb.mMass = gParticleMass;
		pb.mWorldSpaceVelocity = gVelocity + (randomValues1 * velocityMagnitude * gVelVariance);
		pb.mLifeSpan = gLifeSpan;
		pb.mAge = pb.mLifeSpan;
		pb.mStartSize = gOffsetStartSize;
		pb.mEndSize = gOffsetEndSize;
        pb.mStartColor = gOffsetStartColor;
        pb.mEndColor = gOffsetEndColor;
        pb.mAcceleration = gAcceleration;

		uint index = gDeadListToAllocFrom.Consume();
		gParticleBufferA[index] = pa;
		gParticleBufferB[index] = pb;
	}
}