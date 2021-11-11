//---------------------------------------------------------------
// File: RSParticalEmitter.h
// Proj: RenderSystem_DX11
// Info: 描述并具体执行一个粒子发射器的具体机能
// Date: 2021.11.11
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#pragma once

#include "RSCommon.h"

class RSParticalEmitter
{
public:
    RSParticalEmitter(PARTICAL_EMITTER_INFO* _info);
    ~RSParticalEmitter();

    void ResetParticalEmitterInfo(PARTICAL_EMITTER_INFO* _info);

private:
    RS_PARTICAL_EMITTER_INFO mRSParticalEmitterInfo;
    bool mActiveFlg;
    bool mStaticFlg;
};
