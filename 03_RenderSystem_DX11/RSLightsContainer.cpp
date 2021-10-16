//---------------------------------------------------------------
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
    mRootPtr(nullptr), mLightMap({}), mShadowLights({}),
    mShadowLightIndeices({}),
    mAmbientLights({}), mCurrentAmbient({ 0.f,0.f,0.f,0.f })
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
    mLights.clear();
    mShadowLights.clear();
    mShadowLightIndeices.clear();
    mAmbientLights.clear();
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
            mShadowLightIndeices.emplace_back(
                (UINT)(mShadowLights.size() - 1));
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
        for (auto i = mLights.begin(); i != mLights.end(); i++)
        {
            if ((*i) == found->second)
            {
                mLights.erase(i);
                break;
            }
        }
        // TODO delete shadow and shadow index
        for (auto i = mShadowLights.begin();
            i != mShadowLights.end(); i++)
        {
            if ((*i) == found->second)
            {
                mShadowLights.erase(i);
            }
        }
        found->second->ReleaseLightBloom();
        delete found->second;
        mLightMap.erase(found);
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

std::vector<INT>* RSLightsContainer::GetShadowLightIndeices()
{
    return &mShadowLightIndeices;
}

void RSLightsContainer::InsertAmbientLight(std::string&& _name,
    DirectX::XMFLOAT4&& _light)
{
    auto found = mAmbientLights.find(_name);
    if (found == mAmbientLights.end())
    {
        mAmbientLights.insert({ _name,_light });
    }
}

void RSLightsContainer::EraseAmbientLight(std::string&& _name)
{
    auto found = mAmbientLights.find(_name);
    if (found != mAmbientLights.end())
    {
        mAmbientLights.erase(_name);
    }
}

DirectX::XMFLOAT4& RSLightsContainer::GetAmbientLight(
    std::string& _name)
{
    auto found = mAmbientLights.find(_name);
    static DirectX::XMFLOAT4 ambient = {};
    ambient = { 0.f,0.f,0.f,0.f };
    if (found != mAmbientLights.end())
    {
        ambient = mAmbientLights[_name];
    }

    return ambient;
}

void RSLightsContainer::SetCurrentAmbientLight(std::string&& _name)
{
    mCurrentAmbient = GetAmbientLight(_name);
}

DirectX::XMFLOAT4& RSLightsContainer::GetCurrentAmbientLight()
{
    return mCurrentAmbient;
}

void RSLightsContainer::UploadLightBloomDrawCall()
{
    for (auto& light : mLights)
    {
        light->UploadLightDrawCall();
    }
}

void RSLightsContainer::CreateLightBloom(std::string&& _name,
    RS_SUBMESH_DATA&& _meshData, bool _useSolidColor)
{
    GetRSLight(_name)->SetLightBloom(_meshData, _useSolidColor);
}

void RSLightsContainer::CreateLightBloom(std::string& _name,
    RS_SUBMESH_DATA&& _meshData, bool _useSolidColor)
{
    GetRSLight(_name)->SetLightBloom(_meshData, _useSolidColor);
}
