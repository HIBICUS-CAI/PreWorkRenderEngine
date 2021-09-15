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
#include "RSStaticResources.h"
#include <assert.h>

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
    if (!_root || !_texManager) { return false; }

    mRootPtr = _root;
    mTexManagerPtr = _texManager;

    return true;
}

void RSMeshHelper::CleanAndStop()
{

}

RS_SUBMESH_DATA RSMeshHelper::ProcessSubMesh(
    SUBMESH_INFO* _info, LAYOUT_TYPE _layoutType)
{
    assert(_info);

    auto indexBuffer = CreateIndexBuffer(_info->mIndeices);
    auto vertexBuffer = CreateVertexBuffer(_info->mVerteices,
        _layoutType);
    auto texData = CreateTexSrv(_info->mTextures);
    auto material = RS_MATERIAL_INFO();
    if (_info->mMaterial)
    {
        material = CreateSubMeshMaterial(_info->mMaterial);
    }
    else
    {
        material = RefStaticMaterial(_info->mStaticMaterial);
    }

    // TEMP----------------------
    return {};
    // TEMP----------------------
}

ID3D11Buffer* RSMeshHelper::CreateIndexBuffer(
    const std::vector<UINT>* const _indices)
{
    // TEMP----------------------
    return nullptr;
    // TEMP----------------------
}

ID3D11Buffer* RSMeshHelper::CreateVertexBuffer(
    const std::vector<void*>* const _vertices,
    LAYOUT_TYPE _layoutType)
{
    // TEMP----------------------
    return nullptr;
    // TEMP----------------------
}

std::vector<std::string> RSMeshHelper::CreateTexSrv(
    const std::vector<std::string>* const _textures)
{
    // TEMP----------------------
    return {};
    // TEMP----------------------
}

RS_MATERIAL_INFO RSMeshHelper::CreateSubMeshMaterial(
    const MATERIAL_INFO* const _info)
{
    RS_MATERIAL_INFO material = {};
    material.mDiffuseAlbedo = _info->mDiffuseAlbedo;
    material.mFresnelR0 = _info->mFresnelR0;
    material.mShininess = _info->mShininess;

    return material;
}

RS_MATERIAL_INFO RSMeshHelper::RefStaticMaterial(
    std::string& _materialName)
{
    auto material = mRootPtr->StaticResources()->
        GetStaticMaterial(_materialName);
    assert(material);

    return *material;
}
