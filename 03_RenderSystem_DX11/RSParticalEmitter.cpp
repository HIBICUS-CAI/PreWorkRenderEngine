//---------------------------------------------------------------
// File: RSParticalEmitter.cpp
// Proj: RenderSystem_DX11
// Info: 描述并具体执行一个粒子发射器的具体机能
// Date: 2021.11.11
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#include "RSParticalEmitter.h"
#include <assert.h>

static UINT g_ParticalEmitterCounter = 0;

RSParticalEmitter::RSParticalEmitter(PARTICAL_EMITTER_INFO* _info)
    : mRSParticalEmitterInfo({}),
    mActiveFlg(false), mStaticFlg(false)
{
    ResetParticalEmitterInfo(_info);
}

RSParticalEmitter::~RSParticalEmitter()
{

}

void RSParticalEmitter::ResetParticalEmitterInfo(
    PARTICAL_EMITTER_INFO* _info)
{
    if (!_info)
    {
        bool didnt_pass_a_valid_info_to_partical_reset = false;
        assert(didnt_pass_a_valid_info_to_partical_reset);
    }

    mRSParticalEmitterInfo.mEmitterIndex =
        g_ParticalEmitterCounter++;
    mRSParticalEmitterInfo.mEmitNumPerSecond =
        _info->mEmitNumPerSecond;
    mRSParticalEmitterInfo.mNumToEmit = 0;
    mRSParticalEmitterInfo.mAccumulation = 0.f;
    mRSParticalEmitterInfo.mPosition = _info->mPosition;
    mRSParticalEmitterInfo.mVelocity = _info->mVelocity;
    mRSParticalEmitterInfo.mPosVariance = _info->mPosVariance;
    mRSParticalEmitterInfo.mVelVariance = _info->mVelVariance;
    mRSParticalEmitterInfo.mAcceleration = _info->mAcceleration;
    mRSParticalEmitterInfo.mParticalMass = _info->mParticalMass;
    mRSParticalEmitterInfo.mLifeSpan = _info->mLifeSpan;
    mRSParticalEmitterInfo.mOffsetStartSize =
        _info->mOffsetStartSize;
    mRSParticalEmitterInfo.mOffsetEndSize =
        _info->mOffsetEndSize;
    mRSParticalEmitterInfo.mOffsetStartColor =
        _info->mOffsetStartColor;
    mRSParticalEmitterInfo.mOffsetEndColor =
        _info->mOffsetEndColor;
    mRSParticalEmitterInfo.mTextureID =
        (UINT)(1 << (UINT)(_info->mTextureID));
    mRSParticalEmitterInfo.mStreakFlg =
        (_info->mEnableStreak ? 1 : 0);
    mRSParticalEmitterInfo.mMiscFlg = 0;
}

RS_PARTICAL_EMITTER_INFO& RSParticalEmitter::GetRSParticalEmitterInfo()
{
    return mRSParticalEmitterInfo;
}

void RSParticalEmitter::StartParticalEmitter()
{
    mActiveFlg = true;
}

void RSParticalEmitter::PauseParticalEmitter()
{
    mActiveFlg = false;
}
