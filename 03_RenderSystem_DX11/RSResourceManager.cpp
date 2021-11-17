//---------------------------------------------------------------
// File: RSResourceManager.cpp
// Proj: RenderSystem_DX11
// Info: 保存并管理所有被创建的资源
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#include "RSResourceManager.h"
#include "RSRoot_DX11.h"

RSResourceManager::RSResourceManager() :
    mRootPtr(nullptr), mResourceMap({}), mMeshSrvMap({})
{

}

RSResourceManager::~RSResourceManager()
{

}

bool RSResourceManager::StartUp(RSRoot_DX11* _root)
{
    if (!_root) { return false; }

    mRootPtr = _root;

    return true;
}

void RSResourceManager::CleanAndStop()
{
    for (auto& meshSrv : mMeshSrvMap)
    {
        SAFE_RELEASE(meshSrv.second);
    }
    for (auto& dataTex : mResourceMap)
    {
        SAFE_RELEASE(dataTex.second.mUav);
        SAFE_RELEASE(dataTex.second.mSrv);
        SAFE_RELEASE(dataTex.second.mDsv);
        SAFE_RELEASE(dataTex.second.mRtv);
        SAFE_RELEASE(dataTex.second.mTexture);
    }
    mMeshSrvMap.clear();
    mResourceMap.clear();
}

void RSResourceManager::AddResource(
    std::string& _name, RS_RESOURCE_INFO& _resource)
{
    if (mResourceMap.find(_name) == mResourceMap.end())
    {
        mResourceMap.insert({ _name,_resource });
    }
}

void RSResourceManager::AddMeshSrv(
    std::string& _name, ID3D11ShaderResourceView* _srv)
{
    if (mMeshSrvMap.find(_name) == mMeshSrvMap.end())
    {
        mMeshSrvMap.insert({ _name,_srv });
    }
}

RS_RESOURCE_INFO* RSResourceManager::GetResourceInfo(
    std::string& _name)
{
    auto found = mResourceMap.find(_name);
    if (found != mResourceMap.end())
    {
        return &(found->second);
    }
    else
    {
        return nullptr;
    }
}

ID3D11ShaderResourceView* RSResourceManager::GetMeshSrv(
    std::string& _name)
{
    auto found = mMeshSrvMap.find(_name);
    if (found != mMeshSrvMap.end())
    {
        return found->second;
    }
    else
    {
        return nullptr;
    }
}

void RSResourceManager::DeleteResource(std::string& _name)
{
    auto found = mResourceMap.find(_name);
    if (found != mResourceMap.end())
    {
        SAFE_RELEASE(found->second.mUav);
        SAFE_RELEASE(found->second.mSrv);
        SAFE_RELEASE(found->second.mDsv);
        SAFE_RELEASE(found->second.mRtv);
        SAFE_RELEASE(found->second.mTexture);
        mResourceMap.erase(found);
    }
}

void RSResourceManager::DeleteMeshSrv(std::string& _name)
{
    auto found = mMeshSrvMap.find(_name);
    if (found != mMeshSrvMap.end())
    {
        SAFE_RELEASE(found->second);
        mMeshSrvMap.erase(found);
    }
}

void RSResourceManager::ClearResources()
{
    for (auto& dataTex : mResourceMap)
    {
        SAFE_RELEASE(dataTex.second.mUav);
        SAFE_RELEASE(dataTex.second.mSrv);
        SAFE_RELEASE(dataTex.second.mDsv);
        SAFE_RELEASE(dataTex.second.mRtv);
        SAFE_RELEASE(dataTex.second.mTexture);
    }
    mResourceMap.clear();
}

void RSResourceManager::ClearMeshSrvs()
{
    for (auto& meshSrv : mMeshSrvMap)
    {
        SAFE_RELEASE(meshSrv.second);
    }
    mMeshSrvMap.clear();
}
