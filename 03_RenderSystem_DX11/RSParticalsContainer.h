//---------------------------------------------------------------
// File: RSParticalsContainer.h
// Proj: RenderSystem_DX11
// Info: 保存并管理所有的粒子发射器
// Date: 2021.11.11
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#pragma once

#include "RSCommon.h"
#include "RSParticalEmitter.h"
#include <unordered_map>

class RSParticalsContainer
{
public:
    RSParticalsContainer();
    ~RSParticalsContainer();

    bool StartUp(class RSRoot_DX11* _root);
    void CleanAndStop();

    RSParticalEmitter* CreateRSParticalEmitter(
        std::string& _name, PARTICAL_EMITTER_INFO* _info);
    void DeleteRSParticalEmitter(std::string& _name);

    RSParticalEmitter* GetRSParticalEmitter(std::string& _name);
    std::vector<RSParticalEmitter>* GetAllParticalEmitters();

    void StartRSParticalEmitter(std::string& _name);
    void PauseRSParticalEmitter(std::string& _name);

private:
    class RSRoot_DX11* mRootPtr;
    std::vector<RSParticalEmitter> mParticalEmitterVec;
    std::unordered_map<std::string, RSParticalEmitter*>
        mParticalEmitterMap;
};
