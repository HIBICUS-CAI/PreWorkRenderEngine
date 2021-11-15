//---------------------------------------------------------------
// File: RSParticlesContainer.cpp
// Proj: RenderSystem_DX11
// Info: 保存并管理所有的粒子发射器
// Date: 2021.11.11
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#include "RSParticlesContainer.h"
#include "RSRoot_DX11.h"

RSParticlesContainer::RSParticlesContainer() :
    mRootPtr(nullptr), mResetFlg(true),
    mParticleEmitterVec({}), mParticleEmitterMap({})
{

}

RSParticlesContainer::~RSParticlesContainer()
{

}

bool RSParticlesContainer::StartUp(RSRoot_DX11* _root)
{
    if (!_root) { return false; }
    mRootPtr = _root;

    return true;
}

void RSParticlesContainer::CleanAndStop()
{
    mParticleEmitterMap.clear();
    mParticleEmitterVec.clear();
}

bool RSParticlesContainer::GetResetFlg()
{
    return mResetFlg;
}

void RSParticlesContainer::ResetRSParticleSystem()
{
    mResetFlg = true;
}

void RSParticlesContainer::FinishResetRSParticleSystem()
{
    mResetFlg = false;
}

RSParticleEmitter* RSParticlesContainer::CreateRSParticleEmitter(
    std::string& _name, PARTICLE_EMITTER_INFO* _info)
{
    auto size = mParticleEmitterVec.size();
    mParticleEmitterVec.push_back(RSParticleEmitter(_info));
    mParticleEmitterMap.insert(
        { _name,&mParticleEmitterVec[size] });

    return &mParticleEmitterVec[size];
}

void RSParticlesContainer::DeleteRSParticleEmitter(
    std::string& _name)
{
    auto found = mParticleEmitterMap.find(_name);
    if (found != mParticleEmitterMap.end())
    {
        RSParticleEmitter* ptr = found->second;
        for (auto i = mParticleEmitterVec.begin();
            i != mParticleEmitterVec.end(); i++)
        {
            if ((&(*i)) == ptr)
            {
                mParticleEmitterVec.erase(i);
                mParticleEmitterMap.erase(found);
            }
        }
    }
}

RSParticleEmitter* RSParticlesContainer::GetRSParticleEmitter(
    std::string& _name)
{
    auto found = mParticleEmitterMap.find(_name);
    if (found != mParticleEmitterMap.end())
    {
        return found->second;
    }
    else
    {
        return nullptr;
    }
}

std::vector<RSParticleEmitter>*
RSParticlesContainer::GetAllParticleEmitters()
{
    return &mParticleEmitterVec;
}

void RSParticlesContainer::StartRSParticleEmitter(
    std::string& _name)
{
    auto found = mParticleEmitterMap.find(_name);
    if (found != mParticleEmitterMap.end())
    {
        found->second->StartParticleEmitter();
    }
}

void RSParticlesContainer::PauseRSParticleEmitter(
    std::string& _name)
{
    auto found = mParticleEmitterMap.find(_name);
    if (found != mParticleEmitterMap.end())
    {
        found->second->PauseParticleEmitter();
    }
}
