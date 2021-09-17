//---------------------------------------------------------------
// File: RSTexturesManager.cpp
// Proj: RenderSystem_DX11
// Info: 保存并管理所有的纹理
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#include "RSTexturesManager.h"
#include "RSRoot_DX11.h"

RSTexturesManager::RSTexturesManager() :
    mRootPtr(nullptr), mDataTexMap({}), mMeshSrvMap({})
{

}

RSTexturesManager::~RSTexturesManager()
{

}

bool RSTexturesManager::StartUp(RSRoot_DX11* _root)
{
    if (!_root) { return false; }

    mRootPtr = _root;

    return true;
}

void RSTexturesManager::CleanAndStop()
{
    for (auto& meshSrv : mMeshSrvMap)
    {
        SAFE_RELEASE(meshSrv.second);
    }
    for (auto& dataTex : mDataTexMap)
    {
        SAFE_RELEASE(dataTex.second.mUav);
        SAFE_RELEASE(dataTex.second.mSrv);
        SAFE_RELEASE(dataTex.second.mDsv);
        SAFE_RELEASE(dataTex.second.mRtv);
        SAFE_RELEASE(dataTex.second.mTexture);
    }
    mMeshSrvMap.clear();
    mDataTexMap.clear();
}

void RSTexturesManager::AddDataTexture(
    std::string& _name, DATA_TEXTURE_INFO& _tex)
{
    if (mDataTexMap.find(_name) == mDataTexMap.end())
    {
        mDataTexMap.insert({ _name,_tex });
    }
}

void RSTexturesManager::AddMeshSrv(
    std::string& _name, ID3D11ShaderResourceView* _srv)
{
    if (mMeshSrvMap.find(_name) == mMeshSrvMap.end())
    {
        mMeshSrvMap.insert({ _name,_srv });
    }
}

DATA_TEXTURE_INFO* RSTexturesManager::GetDataTexInfo(
    std::string& _name)
{
    auto found = mDataTexMap.find(_name);
    if (found != mDataTexMap.end())
    {
        return &(found->second);
    }
    else
    {
        return nullptr;
    }
}

ID3D11ShaderResourceView* RSTexturesManager::GetMeshSrv(
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

void RSTexturesManager::DeleteDataTex(std::string& _name)
{
    auto found = mDataTexMap.find(_name);
    if (found != mDataTexMap.end())
    {
        SAFE_RELEASE(found->second.mUav);
        SAFE_RELEASE(found->second.mSrv);
        SAFE_RELEASE(found->second.mDsv);
        SAFE_RELEASE(found->second.mRtv);
        SAFE_RELEASE(found->second.mTexture);
        mDataTexMap.erase(found);
    }
}

void RSTexturesManager::DeleteMeshSrv(std::string& _name)
{
    auto found = mMeshSrvMap.find(_name);
    if (found != mMeshSrvMap.end())
    {
        SAFE_RELEASE(found->second);
        mMeshSrvMap.erase(found);
    }
}

void RSTexturesManager::ClearDataTexs()
{
    for (auto& dataTex : mDataTexMap)
    {
        SAFE_RELEASE(dataTex.second.mUav);
        SAFE_RELEASE(dataTex.second.mSrv);
        SAFE_RELEASE(dataTex.second.mDsv);
        SAFE_RELEASE(dataTex.second.mRtv);
        SAFE_RELEASE(dataTex.second.mTexture);
    }
    mDataTexMap.clear();
}

void RSTexturesManager::ClearMeshSrvs()
{
    for (auto& meshSrv : mMeshSrvMap)
    {
        SAFE_RELEASE(meshSrv.second);
    }
    mMeshSrvMap.clear();
}
