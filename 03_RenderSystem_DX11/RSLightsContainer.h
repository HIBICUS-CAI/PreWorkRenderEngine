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
#include <unordered_map>

class RSLightsContainer
{
public:
    RSLightsContainer();
    ~RSLightsContainer();

    bool StartUp(class RSRoot_DX11* _root);
    void CleanAndStop();

    class RSLight* CreateRSLight(std::string& _name, LIGHT_INFO* _info);
    class RSLight* GetRSLight(std::string& _name);
    RS_LIGHT_INFO* GetRSLightInfo(std::string& _name);
    void DeleteRSLight(std::string& _name);

    bool CreateLightCameraFor(std::string& _name, CAM_INFO* _info);

    void InsertAmbientLight(std::string&& _name,
        DirectX::XMFLOAT4&& _light);
    void EraseAmbientLight(std::string&& _name);
    void SetCurrentAmbientLight(std::string&& _name);
    DirectX::XMFLOAT4& GetCurrentAmbientLight();

    std::vector<class RSLight*>* GetLights();
    std::vector<class RSLight*>* GetShadowLights();
    std::vector<INT>* GetShadowLightIndeices();

    void CreateLightBloom(std::string&& _name,
        RS_SUBMESH_DATA&& _meshData);
    void CreateLightBloom(std::string& _name,
        RS_SUBMESH_DATA&& _meshData);
    void UploadLightBloomDrawCall();

private:
    DirectX::XMFLOAT4& GetAmbientLight(std::string& _name);

private:
    class RSRoot_DX11* mRootPtr;

    std::unordered_map<std::string, class RSLight*> mLightMap;
    std::unordered_map<std::string, DirectX::XMFLOAT4> mAmbientLights;

    DirectX::XMFLOAT4 mCurrentAmbient;

    std::vector<class RSLight*> mLights;
    std::vector<class RSLight*> mShadowLights;
    std::vector<INT> mShadowLightIndeices;
};

