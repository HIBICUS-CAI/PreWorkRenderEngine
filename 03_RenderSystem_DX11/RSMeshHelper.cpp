//---------------------------------------------------------------
// File: RSMeshHelper.cpp
// Proj: RenderSystem_DX11
// Info: 提供对SubMesh转换为DirectX可识别形式的方法
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#include "RSMeshHelper.h"
#include "RSRoot_DX11.h"
#include "RSTexturesManager.h"

RSMeshHelper::RSMeshHelper() :
    mRootPtr(nullptr), mTexManagerPtr(nullptr)
{

}

RSMeshHelper::~RSMeshHelper()
{

}

bool RSMeshHelper::StartUp(
    RSRoot_DX11* _root, RSTexturesManager* _texManager)
{
    // TEMP----------------------
    return true;
    // TEMP----------------------
}

void RSMeshHelper::CleanAndStop()
{

}

RS_SUBMESH_DATA RSMeshHelper::ProcessSubMesh(
    SUBMESH_INFO* _info, LAYOUT_TYPE _layoutType)
{
    // TEMP----------------------
    return {};
    // TEMP----------------------
}

ID3D11Buffer* RSMeshHelper::CreateIndexBuffer(
    std::vector<UINT>* _indices)
{
    // TEMP----------------------
    return nullptr;
    // TEMP----------------------
}

ID3D11Buffer* RSMeshHelper::CreateVertexBuffer(
    std::vector<void*>* _vertices)
{
    // TEMP----------------------
    return nullptr;
    // TEMP----------------------
}

std::string RSMeshHelper::CreateTexSrv(
    std::vector<std::string>* _textures)
{
    // TEMP----------------------
    return {};
    // TEMP----------------------
}

RSMaterialNeedToMove RSMeshHelper::CreateSubMeshMaterial(
    MATERIAL_INFO* _info)
{
    // TEMP----------------------
    return {};
    // TEMP----------------------
}

RSMaterialNeedToMove RSMeshHelper::RefStaticMaterial(
    std::string& _materialName)
{
    // TEMP----------------------
    return {};
    // TEMP----------------------
}
