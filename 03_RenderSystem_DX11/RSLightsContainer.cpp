﻿//---------------------------------------------------------------
// File: RSLightsContainer.cpp
// Proj: RenderSystem_DX11
// Info: 对所有的光源进行引用及简单处理
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#include "RSLightsContainer.h"
#include "RSCamerasContainer.h"
#include "RSRoot_DX11.h"
#include "RSLight.h"
#include <algorithm>

RSLightsContainer::RSLightsContainer() :
    mRootPtr(nullptr), mLightMap({}), mShadowLights({})
{

}

RSLightsContainer::~RSLightsContainer()
{

}

bool RSLightsContainer::StartUp(RSRoot_DX11* _root)
{
    if (!_root) { return false; }

    mRootPtr = _root;

    return true;
}

void RSLightsContainer::CleanAndStop()
{
    for (auto& light : mLightMap)
    {
        delete light.second;
    }
    mLightMap.clear();
}

bool LightLessCompare(RSLight* a, RSLight* b)
{
    return (UINT)a->GetRSLightType() < (UINT)b->GetRSLightType();
}

RSLight* RSLightsContainer::CreateRSLight(
    std::string& _name, LIGHT_INFO* _info)
{
    if (!_info) { return nullptr; }

    if (mLightMap.find(_name) == mLightMap.end())
    {
        RSLight* light = new RSLight(_info);
        mLightMap.insert({ _name,light });
        mLights.emplace_back(light);
        std::sort(mLights.begin(), mLights.end(),
            LightLessCompare);
        if (_info->mWithShadow)
        {
            mShadowLights.emplace_back(light);
        }
    }

    return mLightMap[_name];
}

RSLight* RSLightsContainer::GetRSLight(std::string& _name)
{
    auto found = mLightMap.find(_name);
    if (found != mLightMap.end())
    {
        return found->second;
    }
    else
    {
        return nullptr;
    }
}

RS_LIGHT_INFO* RSLightsContainer::GetRSLightInfo(
    std::string& _name)
{
    auto found = mLightMap.find(_name);
    if (found != mLightMap.end())
    {
        return found->second->GetRSLightInfo();
    }
    else
    {
        return nullptr;
    }
}

void RSLightsContainer::DeleteRSLight(std::string& _name)
{
    auto found = mLightMap.find(_name);
    if (found != mLightMap.end())
    {
        mLightMap.erase(found);
        for (auto i = mLights.begin(); i != mLights.end(); i++)
        {
            if ((*i) == found->second)
            {
                mLights.erase(i);
                break;
            }
        }
        for (auto i = mShadowLights.begin();
            i != mShadowLights.end(); i++)
        {
            if ((*i) == found->second)
            {
                mShadowLights.erase(i);
            }
        }
        delete found->second;
    }
}

bool RSLightsContainer::CreateLightCameraFor(
    std::string& _name, CAM_INFO* _info)
{
    auto found = mLightMap.find(_name);
    if (found != mLightMap.end())
    {
        auto cam = found->second->CreateLightCamera(
            _name, _info, mRootPtr->CamerasContainer());
        if (cam)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

std::vector<RSLight*>* RSLightsContainer::GetLights()
{
    return &mLights;
}

std::vector<RSLight*>* RSLightsContainer::GetShadowLights()
{
    return &mShadowLights;
}
