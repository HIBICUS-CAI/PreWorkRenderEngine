//---------------------------------------------------------------
// File: RSParticleEmitter.h
// Proj: RenderSystem_DX11
// Info: ����������ִ��һ�����ӷ������ľ������
// Date: 2021.11.11
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#pragma once

#include "RSCommon.h"

class RSParticleEmitter
{
public:
    RSParticleEmitter(PARTICLE_EMITTER_INFO* _info);
    ~RSParticleEmitter();

    void ResetParticleEmitterInfo(PARTICLE_EMITTER_INFO* _info);

    RS_PARTICLE_EMITTER_INFO& GetRSParticleEmitterInfo();

    void StartParticleEmitter();
    void PauseParticleEmitter();

private:
    RS_PARTICLE_EMITTER_INFO mRSParticleEmitterInfo;
    bool mActiveFlg;
    bool mStaticFlg;
};
