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
    // TEMP----------------------
    return true;
    // TEMP----------------------
}

void RSTexturesManager::CleanAndStop()
{

}

void RSTexturesManager::AddDataTexture(
    std::string& _name, DATA_TEXTURE_INFO&)
{

}

void RSTexturesManager::AddMeshSrv(
    std::string& _name, ID3D11ShaderResourceView* _srv)
{

}

DATA_TEXTURE_INFO* RSTexturesManager::GetDataTexInfo(
    std::string& _name)
{
    // TEMP----------------------
    return nullptr;
    // TEMP----------------------
}

ID3D11ShaderResourceView* RSTexturesManager::GetMeshSrv(
    std::string& _name)
{
    // TEMP----------------------
    return nullptr;
    // TEMP----------------------
}

void RSTexturesManager::DeleteDataTex(std::string& _name)
{

}

void RSTexturesManager::DeleteMeshSrv(std::string& _name)
{

}

void RSTexturesManager::ClearDataTexs()
{

}

void RSTexturesManager::ClearMeshSrvs()
{

}
