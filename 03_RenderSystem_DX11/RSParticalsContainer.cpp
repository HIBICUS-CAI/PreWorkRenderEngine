//---------------------------------------------------------------
// File: RSParticalsContainer.cpp
// Proj: RenderSystem_DX11
// Info: 保存并管理所有的粒子发射器
// Date: 2021.11.11
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#include "RSParticalsContainer.h"
#include "RSRoot_DX11.h"

RSParticalsContainer::RSParticalsContainer() :
    mRootPtr(nullptr), mParticalEmitterVec({}),
    mParticalEmitterMap({})
{

}

RSParticalsContainer::~RSParticalsContainer()
{

}

bool RSParticalsContainer::StartUp(RSRoot_DX11* _root)
{
    if (!_root) { return false; }
    mRootPtr = _root;

    return true;
}

void RSParticalsContainer::CleanAndStop()
{
    mParticalEmitterMap.clear();
    mParticalEmitterVec.clear();
}

RSParticalEmitter* RSParticalsContainer::CreateRSParticalEmitter(
    std::string& _name, PARTICAL_EMITTER_INFO* _info)
{
    auto size = mParticalEmitterVec.size();
    mParticalEmitterVec.resize(size + 1);
    mParticalEmitterVec[size] = RSParticalEmitter(_info);
    mParticalEmitterMap.insert(
        { _name,&mParticalEmitterVec[size] });

    return &mParticalEmitterVec[size];
}

void RSParticalsContainer::DeleteRSParticalEmitter(
    std::string& _name)
{
    auto found = mParticalEmitterMap.find(_name);
    if (found != mParticalEmitterMap.end())
    {
        RSParticalEmitter* ptr = found->second;
        for (auto i = mParticalEmitterVec.begin();
            i != mParticalEmitterVec.end(); i++)
        {
            if ((&(*i)) == ptr)
            {
                mParticalEmitterVec.erase(i);
                mParticalEmitterMap.erase(found);
            }
        }
    }
}

RSParticalEmitter* RSParticalsContainer::GetRSParticalEmitter(
    std::string& _name)
{
    auto found = mParticalEmitterMap.find(_name);
    if (found != mParticalEmitterMap.end())
    {
        return found->second;
    }
    else
    {
        return nullptr;
    }
}

std::vector<RSParticalEmitter>*
RSParticalsContainer::GetAllParticalEmitters()
{
    return &mParticalEmitterVec;
}

void RSParticalsContainer::StartRSParticalEmitter(
    std::string& _name)
{
    auto found = mParticalEmitterMap.find(_name);
    if (found != mParticalEmitterMap.end())
    {
        found->second->StartParticalEmitter();
    }
}

void RSParticalsContainer::PauseRSParticalEmitter(
    std::string& _name)
{
    auto found = mParticalEmitterMap.find(_name);
    if (found != mParticalEmitterMap.end())
    {
        found->second->PauseParticalEmitter();
    }
}
