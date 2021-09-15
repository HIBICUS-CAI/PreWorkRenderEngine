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
#include "RSDevices.h"
#include "RSTexturesManager.h"
#include "RSStaticResources.h"
#include <assert.h>
#include <DirectXTK\WICTextureLoader.h>
#include <DirectXTK\DDSTextureLoader.h>

RSMeshHelper::RSMeshHelper() :
    mRootPtr(nullptr), mTexManagerPtr(nullptr), mDevicesPtr(nullptr)
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
    mDevicesPtr = _root->Devices();

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
    ID3D11Buffer* indexBuffer = nullptr;
    D3D11_BUFFER_DESC ibd = {};
    D3D11_SUBRESOURCE_DATA initData = {};
    ZeroMemory(&ibd, sizeof(ibd));
    ZeroMemory(&initData, sizeof(initData));
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth =
        (UINT)(sizeof(UINT) * _indices->size());
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;

    initData.pSysMem = &((*_indices)[0]);

    HRESULT hr = S_OK;
    hr = mDevicesPtr->GetDevice()->
        CreateBuffer(&ibd, &initData, &indexBuffer);
    if (SUCCEEDED(hr))
    {
        return indexBuffer;
    }
    else
    {
        return nullptr;
    }
}

ID3D11Buffer* RSMeshHelper::CreateVertexBuffer(
    const std::vector<void*>* const _vertices,
    LAYOUT_TYPE _layoutType)
{
    std::vector<VertexType::BasicVertex> basic = {};
    std::vector<VertexType::ColorVertex> color = {};
    std::vector<VertexType::TangentVertex> tangent = {};

    switch (_layoutType)
    {
    case LAYOUT_TYPE::NORMAL_COLOR:
        for (auto& vert : *_vertices)
        {
            color.emplace_back(*(VertexType::ColorVertex*)vert);
        }
        break;
    case LAYOUT_TYPE::NORMAL_TEX:
        for (auto& vert : *_vertices)
        {
            basic.emplace_back(*(VertexType::BasicVertex*)vert);
        }
        break;
    case LAYOUT_TYPE::NORMAL_TANGENT_TEX:
        for (auto& vert : *_vertices)
        {
            tangent.emplace_back(*(VertexType::TangentVertex*)vert);
        }
        break;
    default:
        return nullptr;
    }

    ID3D11Buffer* vertexBuffer = nullptr;
    D3D11_BUFFER_DESC vbd = {};
    D3D11_SUBRESOURCE_DATA initData = {};
    ZeroMemory(&vbd, sizeof(vbd));
    ZeroMemory(&initData, sizeof(initData));
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    switch (_layoutType)
    {
    case LAYOUT_TYPE::NORMAL_COLOR:
        vbd.ByteWidth =
            (UINT)(sizeof(VertexType::ColorVertex) * color.size());
        break;
    case LAYOUT_TYPE::NORMAL_TEX:
        vbd.ByteWidth =
            (UINT)(sizeof(VertexType::BasicVertex) * basic.size());
        break;
    case LAYOUT_TYPE::NORMAL_TANGENT_TEX:
        vbd.ByteWidth =
            (UINT)(sizeof(VertexType::TangentVertex) * tangent.size());
        break;
    default:
        return nullptr;
    }
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;

    switch (_layoutType)
    {
    case LAYOUT_TYPE::NORMAL_COLOR:
        initData.pSysMem = &color[0];
        break;
    case LAYOUT_TYPE::NORMAL_TEX:
        initData.pSysMem = &basic[0];
        break;
    case LAYOUT_TYPE::NORMAL_TANGENT_TEX:
        initData.pSysMem = &tangent[0];
        break;
    default:
        return nullptr;
    }

    HRESULT hr = S_OK;
    hr = mDevicesPtr->GetDevice()->
        CreateBuffer(&vbd, &initData, &vertexBuffer);
    if (SUCCEEDED(hr))
    {
        return vertexBuffer;
    }
    else
    {
        return nullptr;
    }
}

std::vector<std::string> RSMeshHelper::CreateTexSrv(
    const std::vector<std::string>* const _textures)
{
    std::vector<std::string> texVec = {};
    std::wstring wstr = L"";
    std::string name = "";
    HRESULT hr = S_OK;
    ID3D11ShaderResourceView* srv = nullptr;

    for (auto& tex : *_textures)
    {
        wstr = std::wstring(tex.begin(), tex.end());
        wstr = L".\\Textures\\" + wstr;
        if (tex.find(".dds") != std::string::npos ||
            tex.find(".DDS") != std::string::npos)
        {
            hr = DirectX::CreateDDSTextureFromFile(
                mDevicesPtr->GetDevice(),
                wstr.c_str(), nullptr, &srv);
            if (SUCCEEDED(hr))
            {
                name = tex;
                mTexManagerPtr->AddMeshSrv(name, srv);
                texVec.emplace_back(name);
            }
            else
            {
                bool texture_load_fail = false;
                assert(texture_load_fail);
            }
        }
        else
        {
            hr = DirectX::CreateWICTextureFromFile(
                mDevicesPtr->GetDevice(),
                wstr.c_str(), nullptr, &srv);
            if (SUCCEEDED(hr))
            {
                name = tex;
                mTexManagerPtr->AddMeshSrv(name, srv);
                texVec.emplace_back(name);
            }
            else
            {
                bool texture_load_fail = false;
                assert(texture_load_fail);
            }
        }
    }

    return texVec;
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
