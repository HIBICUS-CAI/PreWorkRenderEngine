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

class RSResourceManager
{
public:
    RSResourceManager();
    ~RSResourceManager();

    bool StartUp(class RSRoot_DX11* _root);
    void CleanAndStop();

    void AddResource(
        std::string& _name, RS_RESOURCE_INFO& _resource);
    void AddMeshSrv(
        std::string& _name, ID3D11ShaderResourceView* _srv);
    RS_RESOURCE_INFO* GetResourceInfo(std::string& _name);
    ID3D11ShaderResourceView* GetMeshSrv(std::string& _name);
    void DeleteResource(std::string& _name);
    void DeleteMeshSrv(std::string& _name);
    void ClearResources();
    void ClearMeshSrvs();

private:
    class RSRoot_DX11* mRootPtr;
    std::unordered_map<std::string, RS_RESOURCE_INFO>
        mResourceMap;
    std::unordered_map<std::string, ID3D11ShaderResourceView*>
        mMeshSrvMap;
};
