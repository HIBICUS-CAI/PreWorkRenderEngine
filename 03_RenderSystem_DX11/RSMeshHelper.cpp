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

    RS_SUBMESH_DATA data = {};

    auto type = _info->mTopologyType;
    switch (type)
    {
    case TOPOLOGY_TYPE::POINTLIST:
        data.mTopologyType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
        break;
    case TOPOLOGY_TYPE::LINELIST:
        data.mTopologyType = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
        break;
    case TOPOLOGY_TYPE::LINESTRIP:
        data.mTopologyType = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
        break;
    case TOPOLOGY_TYPE::TRIANGLELIST:
        data.mTopologyType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        break;
    case TOPOLOGY_TYPE::TRIANGLESTRIP:
        data.mTopologyType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        break;
    default:
        data.mTopologyType = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
        break;
    }
    data.mLayout = RefStaticInputLayout(_layoutType);
    data.mIndexBuffer = CreateIndexBuffer(_info->mIndeices);
    data.mVertexBuffer = CreateVertexBuffer(_info->mVerteices,
        _layoutType);
    data.mTextures = CreateTexSrv(_info->mTextures);
    data.mMaterial = RS_MATERIAL_INFO();
    if (_info->mMaterial)
    {
        data.mMaterial = CreateSubMeshMaterial(_info->mMaterial);
    }
    else
    {
        data.mMaterial = RefStaticMaterial(_info->mStaticMaterial);
    }

    return data;
}

ID3D11InputLayout* RSMeshHelper::RefStaticInputLayout(
    LAYOUT_TYPE _layoutType)
{
    ID3D11InputLayout* layout = nullptr;
    static std::string name = "";
    switch (_layoutType)
    {
    case LAYOUT_TYPE::NORMAL_COLOR:
        // TEMP--------------------
        name = "";
        // TEMP--------------------
        layout = mRootPtr->StaticResources()->
            GetStaticInputLayout(name);
        break;
    case LAYOUT_TYPE::NORMAL_TEX:
        // TEMP--------------------
        name = "";
        // TEMP--------------------
        layout = mRootPtr->StaticResources()->
            GetStaticInputLayout(name);
        break;
    case LAYOUT_TYPE::NORMAL_TANGENT_TEX:
        // TEMP--------------------
        name = "";
        // TEMP--------------------
        layout = mRootPtr->StaticResources()->
            GetStaticInputLayout(name);
        break;
    default:
        break;
    }

    return layout;
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
