//---------------------------------------------------------------
// File: RSParticalEmitter.cpp
// Proj: RenderSystem_DX11
// Info: 描述并具体执行一个粒子发射器的具体机能
// Date: 2021.11.11
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#include "RSParticalEmitter.h"

RSParticalEmitter::RSParticalEmitter(PARTICAL_EMITTER_INFO* _info)
    : mRSParticalEmitterInfo({}),
    mActiveFlg(false), mStaticFlg(false)
{

}

RSParticalEmitter::~RSParticalEmitter()
{

}

void RSParticalEmitter::ResetParticalEmitterInfo(
    PARTICAL_EMITTER_INFO* _info)
{

}
