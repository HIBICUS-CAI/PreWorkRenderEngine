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
    mRootPtr(nullptr), mTexManagerPtr(nullptr),
    mDevicesPtr(nullptr), mGeoGeneratorPtr(nullptr)
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
    mGeoGeneratorPtr = new RSGeometryGenerator(this);

    return true;
}

void RSMeshHelper::CleanAndStop()
{
    if (mGeoGeneratorPtr)
    {
        delete mGeoGeneratorPtr;
        mGeoGeneratorPtr = nullptr;
    }
}

RSGeometryGenerator* RSMeshHelper::GeoGenerate()
{
    return mGeoGeneratorPtr;
}

void RSMeshHelper::ProcessSubMesh(
    RS_SUBMESH_DATA* _result,
    SUBMESH_INFO* _info, LAYOUT_TYPE _layoutType)
{
    assert(_info);

    auto type = _info->mTopologyType;
    switch (type)
    {
    case TOPOLOGY_TYPE::POINTLIST:
        _result->mTopologyType =
            D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
        break;
    case TOPOLOGY_TYPE::LINELIST:
        _result->mTopologyType =
            D3D_PRIMITIVE_TOPOLOGY_LINELIST;
        break;
    case TOPOLOGY_TYPE::LINESTRIP:
        _result->mTopologyType =
            D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
        break;
    case TOPOLOGY_TYPE::TRIANGLELIST:
        _result->mTopologyType =
            D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        break;
    case TOPOLOGY_TYPE::TRIANGLESTRIP:
        _result->mTopologyType =
            D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        break;
    default:
        _result->mTopologyType =
            D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
        break;
    }
    _result->mLayout = RefStaticInputLayout(_layoutType);
    _result->mIndexBuffer = CreateIndexBuffer(_info->mIndeices);
    _result->mVertexBuffer = CreateVertexBuffer(_info->mVerteices,
        _layoutType);
    CreateTexSrv(_result, _info->mTextures);
    if (_info->mMaterial)
    {
        CreateSubMeshMaterial(_result, _info->mMaterial);
    }
    else
    {
        RefStaticMaterial(_result, _info->mStaticMaterial);
    }
}

ID3D11InputLayout* RSMeshHelper::RefStaticInputLayout(
    LAYOUT_TYPE _layoutType)
{
    ID3D11InputLayout* layout = nullptr;
    static std::string name = "";
    switch (_layoutType)
    {
    case LAYOUT_TYPE::NORMAL_COLOR:
        name = "ColorVertex";
        break;
    case LAYOUT_TYPE::NORMAL_TEX:
        name = "BasicVertex";
        break;
    case LAYOUT_TYPE::NORMAL_TANGENT_TEX:
        name = "TangentVertex";
        break;
    default:
        return nullptr;
    }
    layout = mRootPtr->StaticResources()->
        GetStaticInputLayout(name);

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
    const void* const _vertices,
    LAYOUT_TYPE _layoutType)
{
    std::vector<VertexType::BasicVertex>* basic = nullptr;
    std::vector<VertexType::ColorVertex>* color = nullptr;
    std::vector<VertexType::TangentVertex>* tangent = nullptr;
    UINT size = 0;
    UINT vertexSize = 0;
    void* vertArray = nullptr;

    switch (_layoutType)
    {
    case LAYOUT_TYPE::NORMAL_COLOR:
        color =
            (std::vector<VertexType::ColorVertex>*)_vertices;
        size = (UINT)color->size();
        vertexSize = (UINT)sizeof(VertexType::ColorVertex);
        vertArray = &((*color)[0]);
        break;
    case LAYOUT_TYPE::NORMAL_TEX:
        basic =
            (std::vector<VertexType::BasicVertex>*)_vertices;
        size = (UINT)basic->size();
        vertexSize = (UINT)sizeof(VertexType::BasicVertex);
        vertArray = &((*basic)[0]);
        break;
    case LAYOUT_TYPE::NORMAL_TANGENT_TEX:
        tangent =
            (std::vector<VertexType::TangentVertex>*)_vertices;
        size = (UINT)tangent->size();
        vertexSize = (UINT)sizeof(VertexType::TangentVertex);
        vertArray = &((*tangent)[0]);
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
    vbd.ByteWidth = vertexSize * size;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;

    initData.pSysMem = vertArray;

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

void RSMeshHelper::CreateTexSrv(
    RS_SUBMESH_DATA* _result,
    const std::vector<std::string>* const _textures)
{
    std::vector<std::string>* texVec = &(_result->mTextures);
    static std::wstring wstr = L"";
    static std::string name = "";
    static HRESULT hr = S_OK;
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
                texVec->emplace_back(name);
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
                texVec->emplace_back(name);
            }
            else
            {
                bool texture_load_fail = false;
                assert(texture_load_fail);
            }
        }
    }
}

void RSMeshHelper::CreateSubMeshMaterial(
    RS_SUBMESH_DATA* _result,
    const MATERIAL_INFO* const _info)
{
    assert(_info);

    RS_MATERIAL_INFO* material = &(_result->mMaterial);
    material->mDiffuseAlbedo = _info->mDiffuseAlbedo;
    material->mFresnelR0 = _info->mFresnelR0;
    material->mShininess = _info->mShininess;
}

void RSMeshHelper::RefStaticMaterial(
    RS_SUBMESH_DATA* _result,
    std::string& _materialName)
{
    auto material = mRootPtr->StaticResources()->
        GetStaticMaterial(_materialName);
    assert(material);

    _result->mMaterial.mDiffuseAlbedo = material->mDiffuseAlbedo;
    _result->mMaterial.mFresnelR0 = material->mFresnelR0;
    _result->mMaterial.mShininess = material->mShininess;
}

void RSMeshHelper::ReleaseSubMesh(RS_SUBMESH_DATA& _result)
{
    SAFE_RELEASE(_result.mIndexBuffer);
    SAFE_RELEASE(_result.mVertexBuffer);
}

RSGeometryGenerator::RSGeometryGenerator(RSMeshHelper* _helper) :
    mMeshHelperPtr(_helper) {}

RSGeometryGenerator::~RSGeometryGenerator() {}

RS_SUBMESH_DATA RSGeometryGenerator::CreateBox(
    float _width, float _height, float _depth, UINT _diviNum,
    std::string&& _layoutName, bool _useVertexColor,
    DirectX::XMFLOAT4&& _vertColor, std::string&& _texColorName)
{
    return {};
}

RS_SUBMESH_DATA RSGeometryGenerator::CreateSphere(
    float _radius, UINT _sliceCount, UINT _stackCount,
    std::string&& _layoutName, bool _useVertexColor,
    DirectX::XMFLOAT4&& _vertColor, std::string&& _texColorName)
{
    return {};
}

RS_SUBMESH_DATA RSGeometryGenerator::CreateGeometrySphere(
    float _radius, UINT _diviNum,
    std::string&& _layoutName, bool _useVertexColor,
    DirectX::XMFLOAT4&& _vertColor, std::string&& _texColorName)
{
    return {};
}

RS_SUBMESH_DATA RSGeometryGenerator::CreateCylinder(
    float _bottomRadius, float _topRadius, float _height,
    UINT _sliceCount, UINT _stackCount,
    std::string&& _layoutName, bool _useVertexColor,
    DirectX::XMFLOAT4&& _vertColor, std::string&& _texColorName)
{
    return {};
}

RS_SUBMESH_DATA RSGeometryGenerator::CreateGrid(
    float _width, float _depth, UINT _rowCount, UINT _colCount,
    std::string&& _layoutName, bool _useVertexColor,
    DirectX::XMFLOAT4&& _vertColor, std::string&& _texColorName)
{
    return {};
}
