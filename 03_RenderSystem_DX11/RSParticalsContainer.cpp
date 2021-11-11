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
    return nullptr;
}

void RSParticalsContainer::DeleteRSParticalEmitter(
    std::string& _name)
{

}

RSParticalEmitter* RSParticalsContainer::GetRSParticalEmitter(
    std::string& _name)
{
    return nullptr;
}

std::vector<RSParticalEmitter>* 
RSParticalsContainer::GetAllParticalEmitters()
{
    return nullptr;
}

void RSParticalsContainer::StartRSParticalEmitter(std::string& _name)
{

}

void RSParticalsContainer::PauseRSParticalEmitter(std::string& _name)
{

}
