//---------------------------------------------------------------
// File: RSResourceManager.h
// Proj: RenderSystem_DX11
// Info: 保存并管理所有被创建的资源
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#pragma once

#include "RSCommon.h"
#include <unordered_map>

class RSTexturesManager
{
public:
    RSTexturesManager();
    ~RSTexturesManager();

    bool StartUp(class RSRoot_DX11* _root);
    void CleanAndStop();

    void AddDataTexture(
        std::string& _name, DATA_TEXTURE_INFO& _tex);
    void AddMeshSrv(
        std::string& _name, ID3D11ShaderResourceView* _srv);
    DATA_TEXTURE_INFO* GetDataTexInfo(std::string& _name);
    ID3D11ShaderResourceView* GetMeshSrv(std::string& _name);
    void DeleteDataTex(std::string& _name);
    void DeleteMeshSrv(std::string& _name);
    void ClearDataTexs();
    void ClearMeshSrvs();

private:
    class RSRoot_DX11* mRootPtr;
    std::unordered_map<std::string, DATA_TEXTURE_INFO>
        mDataTexMap;
    std::unordered_map<std::string, ID3D11ShaderResourceView*>
        mMeshSrvMap;
};
