//---------------------------------------------------------------
// File: RSParticlesContainer.h
// Proj: RenderSystem_DX11
// Info: 保存并管理所有的粒子发射器
// Date: 2021.11.11
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#pragma once

#include "RSCommon.h"
#include "RSParticleEmitter.h"
#include <unordered_map>

class RSParticlesContainer
{
public:
    RSParticlesContainer();
    ~RSParticlesContainer();

    bool StartUp(class RSRoot_DX11* _root);
    void CleanAndStop();

    bool GetResetFlg();
    void ResetRSParticleSystem();
    void FinishResetRSParticleSystem();

    RSParticleEmitter* CreateRSParticleEmitter(
        std::string& _name, PARTICLE_EMITTER_INFO* _info);
    void DeleteRSParticleEmitter(std::string& _name);

    RSParticleEmitter* GetRSParticleEmitter(std::string& _name);
    std::vector<RSParticleEmitter>* GetAllParticleEmitters();

    void StartRSParticleEmitter(std::string& _name);
    void PauseRSParticleEmitter(std::string& _name);

private:
    class RSRoot_DX11* mRootPtr;

    bool mResetFlg;

    std::vector<RSParticleEmitter> mParticleEmitterVec;
    std::unordered_map<std::string, RSParticleEmitter*>
        mParticleEmitterMap;
};
