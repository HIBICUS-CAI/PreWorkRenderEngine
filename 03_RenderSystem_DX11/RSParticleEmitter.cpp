//---------------------------------------------------------------
// File: RSParticleEmitter.cpp
// Proj: RenderSystem_DX11
// Info: 描述并具体执行一个粒子发射器的具体机能
// Date: 2021.11.11
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#include "RSParticleEmitter.h"
#include <assert.h>

static UINT g_ParticleEmitterCounter = 0;

RSParticleEmitter::RSParticleEmitter(PARTICLE_EMITTER_INFO* _info)
    : mRSParticleEmitterInfo({}),
    mActiveFlg(false), mStaticFlg(false)
{
    ResetParticleEmitterInfo(_info);
}

RSParticleEmitter::~RSParticleEmitter()
{

}

void RSParticleEmitter::ResetParticleEmitterInfo(
    PARTICLE_EMITTER_INFO* _info)
{
    if (!_info)
    {
        bool didnt_pass_a_valid_info_to_particle_reset = false;
        assert(didnt_pass_a_valid_info_to_particle_reset);
    }

    mRSParticleEmitterInfo.mEmitterIndex =
        g_ParticleEmitterCounter++;
    mRSParticleEmitterInfo.mEmitNumPerSecond =
        _info->mEmitNumPerSecond;
    mRSParticleEmitterInfo.mNumToEmit = 0;
    mRSParticleEmitterInfo.mAccumulation = 0.f;
    mRSParticleEmitterInfo.mPosition = _info->mPosition;
    mRSParticleEmitterInfo.mVelocity = _info->mVelocity;
    mRSParticleEmitterInfo.mPosVariance = _info->mPosVariance;
    mRSParticleEmitterInfo.mVelVariance = _info->mVelVariance;
    mRSParticleEmitterInfo.mAcceleration = _info->mAcceleration;
    mRSParticleEmitterInfo.mParticleMass = _info->mParticleMass;
    mRSParticleEmitterInfo.mLifeSpan = _info->mLifeSpan;
    mRSParticleEmitterInfo.mOffsetStartSize =
        _info->mOffsetStartSize;
    mRSParticleEmitterInfo.mOffsetEndSize =
        _info->mOffsetEndSize;
    mRSParticleEmitterInfo.mOffsetStartColor =
        _info->mOffsetStartColor;
    mRSParticleEmitterInfo.mOffsetEndColor =
        _info->mOffsetEndColor;
    mRSParticleEmitterInfo.mTextureID =
        (UINT)(1 << (UINT)(_info->mTextureID));
    mRSParticleEmitterInfo.mStreakFlg =
        (_info->mEnableStreak ? 1 : 0);
    mRSParticleEmitterInfo.mMiscFlg = 0;
}

RS_PARTICLE_EMITTER_INFO& RSParticleEmitter::GetRSParticleEmitterInfo()
{
    return mRSParticleEmitterInfo;
}

void RSParticleEmitter::StartParticleEmitter()
{
    mActiveFlg = true;
}

void RSParticleEmitter::PauseParticleEmitter()
{
    mActiveFlg = false;
}
