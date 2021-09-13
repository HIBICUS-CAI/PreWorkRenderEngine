//---------------------------------------------------------------
// File: RSLightsContainer.cpp
// Proj: RenderSystem_DX11
// Info: 对所有的光源进行引用及简单处理
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#include "RSLightsContainer.h"
#include "RSRoot_DX11.h"

RSLightsContainer::RSLightsContainer() :
    mRootPtr(nullptr), mLightMap({})
{

}

RSLightsContainer::~RSLightsContainer()
{

}

bool RSLightsContainer::StartUp(RSRoot_DX11* _root)
{
    // TEMP----------------------
    return true;
    // TEMP----------------------
}

void RSLightsContainer::CleanAndStop()
{

}

RSLight* RSLightsContainer::CreateRSLight(
    std::string& _name, LIGHT_INFO* _info)
{
    // TEMP----------------------
    return nullptr;
    // TEMP----------------------
}

RSLight* RSLightsContainer::GetRSLight(std::string& _name)
{
    // TEMP----------------------
    return nullptr;
    // TEMP----------------------
}

RS_LIGHT_INFO* RSLightsContainer::GetRSLightInfo(
    std::string& _name)
{
    // TEMP----------------------
    return nullptr;
    // TEMP----------------------
}

void RSLightsContainer::DeleteRSLight(std::string& _name)
{

}
