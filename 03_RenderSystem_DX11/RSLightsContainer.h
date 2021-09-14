//---------------------------------------------------------------
// File: RSLightsContainer.h
// Proj: RenderSystem_DX11
// Info: 对所有的光源进行引用及简单处理
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#pragma once

#include "RSCommon.h"
#include <string>
#include <unordered_map>
#include "RSLight.h"

class RSLightsContainer
{
public:
    RSLightsContainer();
    ~RSLightsContainer();

    bool StartUp(class RSRoot_DX11* _root);
    void CleanAndStop();

    RSLight* CreateRSLight(std::string& _name, LIGHT_INFO* _info);
    RSLight* GetRSLight(std::string& _name);
    RS_LIGHT_INFO* GetRSLightInfo(std::string& _name);
    void DeleteRSLight(std::string& _name);

private:
    class RSRoot_DX11* mRootPtr;

    std::unordered_map<std::string, RSLight> mLightMap;
};

