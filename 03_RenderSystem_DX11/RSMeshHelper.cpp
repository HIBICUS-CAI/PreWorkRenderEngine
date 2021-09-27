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

using namespace DirectX;

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
    _result->mIndexCount = (UINT)_info->mIndeices->size();
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
    LAYOUT_TYPE _layout, bool _useVertexColor,
    DirectX::XMFLOAT4&& _vertColor, std::string&& _texColorName)
{
    RS_SUBMESH_DATA rsd = {};
    SUBMESH_INFO si = {};
    MATERIAL_INFO mi = {};
    std::vector<UINT> indeices = {};
    std::vector<VertexType::BasicVertex> basic = {};
    std::vector<VertexType::TangentVertex> tangent = {};
    std::vector<VertexType::ColorVertex> color = {};
    std::vector<std::string> textures = {};
    std::string str = "";
    float hw = 0.5f * _width;
    float hh = 0.5f * _height;
    float hd = 0.5f * _depth;
    indeices.resize(36);

    switch (_layout)
    {
    case LAYOUT_TYPE::NORMAL_COLOR:
        if (!_useVertexColor)
        {
            bool notUsingVertColor = false;
            assert(notUsingVertColor);
        }
        color.resize(24);
        // front face
        color[0] = { { -hw, -hh, -hd }, { 0.0f, 0.0f, -1.0f }, _vertColor };
        color[1] = { { -hw, +hh, -hd }, { 0.0f, 0.0f, -1.0f }, _vertColor };
        color[2] = { { +hw, +hh, -hd }, { 0.0f, 0.0f, -1.0f }, _vertColor };
        color[3] = { { +hw, -hh, -hd }, { 0.0f, 0.0f, -1.0f }, _vertColor };
        // back face
        color[4] = { { -hw, -hh, +hd }, { 0.0f, 0.0f, 1.0f }, _vertColor };
        color[5] = { { +hw, -hh, +hd }, { 0.0f, 0.0f, 1.0f }, _vertColor };
        color[6] = { { +hw, +hh, +hd }, { 0.0f, 0.0f, 1.0f }, _vertColor };
        color[7] = { { -hw, +hh, +hd }, { 0.0f, 0.0f, 1.0f }, _vertColor };
        // top face
        color[8] = { { -hw, +hh, -hd }, { 0.0f, 1.0f, 0.0f }, _vertColor };
        color[9] = { { -hw, +hh, +hd }, { 0.0f, 1.0f, 0.0f }, _vertColor };
        color[10] = { { +hw, +hh, +hd }, { 0.0f, 1.0f, 0.0f }, _vertColor };
        color[11] = { { +hw, +hh, -hd }, { 0.0f, 1.0f, 0.0f }, _vertColor };
        // bottom face
        color[12] = { { -hw, -hh, -hd }, { 0.0f, -1.0f, 0.0f }, _vertColor };
        color[13] = { { +hw, -hh, -hd }, { 0.0f, -1.0f, 0.0f }, _vertColor };
        color[14] = { { +hw, -hh, +hd }, { 0.0f, -1.0f, 0.0f }, _vertColor };
        color[15] = { { -hw, -hh, +hd }, { 0.0f, -1.0f, 0.0f }, _vertColor };
        // left face
        color[16] = { { -hw, -hh, +hd }, { -1.0f, 0.0f, 0.0f }, _vertColor };
        color[17] = { { -hw, +hh, +hd }, { -1.0f, 0.0f, 0.0f }, _vertColor };
        color[18] = { { -hw, +hh, -hd }, { -1.0f, 0.0f, 0.0f }, _vertColor };
        color[19] = { { -hw, -hh, -hd }, { -1.0f, 0.0f, 0.0f }, _vertColor };
        // right face
        color[20] = { { +hw, -hh, -hd }, { 1.0f, 0.0f, 0.0f }, _vertColor };
        color[21] = { { +hw, +hh, -hd }, { 1.0f, 0.0f, 0.0f }, _vertColor };
        color[22] = { { +hw, +hh, +hd }, { 1.0f, 0.0f, 0.0f }, _vertColor };
        color[23] = { { +hw, -hh, +hd }, { 1.0f, 0.0f, 0.0f }, _vertColor };

        // front face index
        indeices[0] = 0; indeices[1] = 1; indeices[2] = 2;
        indeices[3] = 0; indeices[4] = 2; indeices[5] = 3;
        // back face index
        indeices[6] = 4; indeices[7] = 5; indeices[8] = 6;
        indeices[9] = 4; indeices[10] = 6; indeices[11] = 7;
        // top face index
        indeices[12] = 8; indeices[13] = 9; indeices[14] = 10;
        indeices[15] = 8; indeices[16] = 10; indeices[17] = 11;
        // bottom face index
        indeices[18] = 12; indeices[19] = 13; indeices[20] = 14;
        indeices[21] = 12; indeices[22] = 14; indeices[23] = 15;
        // left face index
        indeices[24] = 16; indeices[25] = 17; indeices[26] = 18;
        indeices[27] = 16; indeices[28] = 18; indeices[29] = 19;
        // right face index
        indeices[30] = 20; indeices[31] = 21; indeices[32] = 22;
        indeices[33] = 20; indeices[34] = 22; indeices[35] = 23;

        if (_diviNum > 6) { _diviNum = 6; }
        for (UINT i = 0; i < _diviNum; i++)
        {
            SubDivide(_layout, &color, &indeices);
        }

        si.mTopologyType = TOPOLOGY_TYPE::TRIANGLELIST;
        si.mVerteices = &color;
        si.mIndeices = &indeices;
        si.mTextures = &textures;
        si.mMaterial = &mi;
        mMeshHelperPtr->ProcessSubMesh(&rsd, &si, _layout);

        break;

    case LAYOUT_TYPE::NORMAL_TEX:
        if (_useVertexColor)
        {
            bool usingVertColor = false;
            assert(usingVertColor);
        }
        basic.resize(24);
        // front face
        basic[0] = { { -hw, -hh, -hd }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } };
        basic[1] = { { -hw, +hh, -hd }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } };
        basic[2] = { { +hw, +hh, -hd }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } };
        basic[3] = { { +hw, -hh, -hd }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } };
        // back face
        basic[4] = { { -hw, -hh, +hd }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } };
        basic[5] = { { +hw, -hh, +hd }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } };
        basic[6] = { { +hw, +hh, +hd }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } };
        basic[7] = { { -hw, +hh, +hd }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } };
        // top face
        basic[8] = { { -hw, +hh, -hd }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } };
        basic[9] = { { -hw, +hh, +hd }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } };
        basic[10] = { { +hw, +hh, +hd }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } };
        basic[11] = { { +hw, +hh, -hd }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } };
        // bottom face
        basic[12] = { { -hw, -hh, -hd }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } };
        basic[13] = { { +hw, -hh, -hd }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } };
        basic[14] = { { +hw, -hh, +hd }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } };
        basic[15] = { { -hw, -hh, +hd }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } };
        // left face
        basic[16] = { { -hw, -hh, +hd }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } };
        basic[17] = { { -hw, +hh, +hd }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } };
        basic[18] = { { -hw, +hh, -hd }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } };
        basic[19] = { { -hw, -hh, -hd }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } };
        // right face
        basic[20] = { { +hw, -hh, -hd }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } };
        basic[21] = { { +hw, +hh, -hd }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } };
        basic[22] = { { +hw, +hh, +hd }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } };
        basic[23] = { { +hw, -hh, +hd }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } };

        // front face index
        indeices[0] = 0; indeices[1] = 1; indeices[2] = 2;
        indeices[3] = 0; indeices[4] = 2; indeices[5] = 3;
        // back face index
        indeices[6] = 4; indeices[7] = 5; indeices[8] = 6;
        indeices[9] = 4; indeices[10] = 6; indeices[11] = 7;
        // top face index
        indeices[12] = 8; indeices[13] = 9; indeices[14] = 10;
        indeices[15] = 8; indeices[16] = 10; indeices[17] = 11;
        // bottom face index
        indeices[18] = 12; indeices[19] = 13; indeices[20] = 14;
        indeices[21] = 12; indeices[22] = 14; indeices[23] = 15;
        // left face index
        indeices[24] = 16; indeices[25] = 17; indeices[26] = 18;
        indeices[27] = 16; indeices[28] = 18; indeices[29] = 19;
        // right face index
        indeices[30] = 20; indeices[31] = 21; indeices[32] = 22;
        indeices[33] = 20; indeices[34] = 22; indeices[35] = 23;

        if (_diviNum > 6) { _diviNum = 6; }
        for (UINT i = 0; i < _diviNum; i++)
        {
            SubDivide(_layout, &basic, &indeices);
        }

        textures.emplace_back(_texColorName);

        si.mTopologyType = TOPOLOGY_TYPE::TRIANGLELIST;
        si.mVerteices = &basic;
        si.mIndeices = &indeices;
        si.mTextures = &textures;
        si.mMaterial = &mi;
        mMeshHelperPtr->ProcessSubMesh(&rsd, &si, _layout);

        break;

    case LAYOUT_TYPE::NORMAL_TANGENT_TEX:
        if (_useVertexColor)
        {
            bool usingVertColor = false;
            assert(usingVertColor);
        }
        tangent.resize(24);
        // front face
        tangent[0] = { { -hw, -hh, -hd }, { 0.0f, 0.0f, -1.0f }, {1.0f, 0.0f, 0.0f}, { 0.0f, 1.0f } };
        tangent[1] = { { -hw, +hh, -hd }, { 0.0f, 0.0f, -1.0f }, {1.0f, 0.0f, 0.0f}, { 0.0f, 0.0f } };
        tangent[2] = { { +hw, +hh, -hd }, { 0.0f, 0.0f, -1.0f }, {1.0f, 0.0f, 0.0f}, { 1.0f, 0.0f } };
        tangent[3] = { { +hw, -hh, -hd }, { 0.0f, 0.0f, -1.0f }, {1.0f, 0.0f, 0.0f}, { 1.0f, 1.0f } };
        // back face
        tangent[4] = { { -hw, -hh, +hd }, { 0.0f, 0.0f, 1.0f }, {-1.0f, 0.0f, 0.0f}, { 1.0f, 1.0f } };
        tangent[5] = { { +hw, -hh, +hd }, { 0.0f, 0.0f, 1.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f, 1.0f } };
        tangent[6] = { { +hw, +hh, +hd }, { 0.0f, 0.0f, 1.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f, 0.0f } };
        tangent[7] = { { -hw, +hh, +hd }, { 0.0f, 0.0f, 1.0f }, {-1.0f, 0.0f, 0.0f}, { 1.0f, 0.0f } };
        // top face
        tangent[8] = { { -hw, +hh, -hd }, { 0.0f, 1.0f, 0.0f }, {1.0f, 0.0f, 0.0f}, { 0.0f, 1.0f } };
        tangent[9] = { { -hw, +hh, +hd }, { 0.0f, 1.0f, 0.0f }, {1.0f, 0.0f, 0.0f}, { 0.0f, 0.0f } };
        tangent[10] = { { +hw, +hh, +hd }, { 0.0f, 1.0f, 0.0f }, {1.0f, 0.0f, 0.0f}, { 1.0f, 0.0f } };
        tangent[11] = { { +hw, +hh, -hd }, { 0.0f, 1.0f, 0.0f }, {1.0f, 0.0f, 0.0f}, { 1.0f, 1.0f } };
        // bottom face
        tangent[12] = { { -hw, -hh, -hd }, { 0.0f, -1.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 1.0f, 1.0f } };
        tangent[13] = { { +hw, -hh, -hd }, { 0.0f, -1.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f, 1.0f } };
        tangent[14] = { { +hw, -hh, +hd }, { 0.0f, -1.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f, 0.0f } };
        tangent[15] = { { -hw, -hh, +hd }, { 0.0f, -1.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 1.0f, 0.0f } };
        // left face
        tangent[16] = { { -hw, -hh, +hd }, { -1.0f, 0.0f, 0.0f }, {0.0f, 0.0f, -1.0f}, { 0.0f, 1.0f } };
        tangent[17] = { { -hw, +hh, +hd }, { -1.0f, 0.0f, 0.0f }, {0.0f, 0.0f, -1.0f}, { 0.0f, 0.0f } };
        tangent[18] = { { -hw, +hh, -hd }, { -1.0f, 0.0f, 0.0f }, {0.0f, 0.0f, -1.0f}, { 1.0f, 0.0f } };
        tangent[19] = { { -hw, -hh, -hd }, { -1.0f, 0.0f, 0.0f }, {0.0f, 0.0f, -1.0f}, { 1.0f, 1.0f } };
        // right face
        tangent[20] = { { +hw, -hh, -hd }, { 1.0f, 0.0f, 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f, 1.0f } };
        tangent[21] = { { +hw, +hh, -hd }, { 1.0f, 0.0f, 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f, 0.0f } };
        tangent[22] = { { +hw, +hh, +hd }, { 1.0f, 0.0f, 0.0f }, {0.0f, 0.0f, 1.0f}, { 1.0f, 0.0f } };
        tangent[23] = { { +hw, -hh, +hd }, { 1.0f, 0.0f, 0.0f }, {0.0f, 0.0f, 1.0f}, { 1.0f, 1.0f } };

        // front face index
        indeices[0] = 0; indeices[1] = 1; indeices[2] = 2;
        indeices[3] = 0; indeices[4] = 2; indeices[5] = 3;
        // back face index
        indeices[6] = 4; indeices[7] = 5; indeices[8] = 6;
        indeices[9] = 4; indeices[10] = 6; indeices[11] = 7;
        // top face index
        indeices[12] = 8; indeices[13] = 9; indeices[14] = 10;
        indeices[15] = 8; indeices[16] = 10; indeices[17] = 11;
        // bottom face index
        indeices[18] = 12; indeices[19] = 13; indeices[20] = 14;
        indeices[21] = 12; indeices[22] = 14; indeices[23] = 15;
        // left face index
        indeices[24] = 16; indeices[25] = 17; indeices[26] = 18;
        indeices[27] = 16; indeices[28] = 18; indeices[29] = 19;
        // right face index
        indeices[30] = 20; indeices[31] = 21; indeices[32] = 22;
        indeices[33] = 20; indeices[34] = 22; indeices[35] = 23;

        if (_diviNum > 6) { _diviNum = 6; }
        for (UINT i = 0; i < _diviNum; i++)
        {
            SubDivide(_layout, &tangent, &indeices);
        }

        textures.emplace_back(_texColorName);

        si.mTopologyType = TOPOLOGY_TYPE::TRIANGLELIST;
        si.mVerteices = &tangent;
        si.mIndeices = &indeices;
        si.mTextures = &textures;
        si.mMaterial = &mi;
        mMeshHelperPtr->ProcessSubMesh(&rsd, &si, _layout);

        break;

    default:
        bool nullLayout = false;
        assert(nullLayout);
    }

    return rsd;
}

RS_SUBMESH_DATA RSGeometryGenerator::CreateSphere(
    float _radius, UINT _sliceCount, UINT _stackCount,
    LAYOUT_TYPE _layout, bool _useVertexColor,
    DirectX::XMFLOAT4&& _vertColor, std::string&& _texColorName)
{
    RS_SUBMESH_DATA rsd = {};
    SUBMESH_INFO si = {};
    MATERIAL_INFO mi = {};
    std::vector<UINT> indeices = {};
    std::vector<VertexType::BasicVertex> basic = {};
    std::vector<VertexType::TangentVertex> tangent = {};
    std::vector<VertexType::ColorVertex> color = {};
    std::vector<std::string> textures = {};

    switch (_layout)
    {
    case LAYOUT_TYPE::NORMAL_COLOR:
    {
        if (!_useVertexColor)
        {
            bool notUsingVertColor = false;
            assert(notUsingVertColor);
        }
        VertexType::ColorVertex topVertex =
        {
            { 0.0f, +_radius, 0.0f }, { 0.0f, +1.0f, 0.0f },
            _vertColor
        };
        VertexType::ColorVertex bottomVertex =
        {
            { 0.0f, -_radius, 0.0f }, { 0.0f, -1.0f, 0.0f },
            _vertColor
        };

        color.emplace_back(topVertex);

        float phiStep = DirectX::XM_PI / _stackCount;
        float thetaStep = 2.0f * DirectX::XM_PI / _sliceCount;

        for (UINT i = 1; i <= _stackCount - 1; i++)
        {
            float phi = i * phiStep;

            for (UINT j = 0; j <= _sliceCount; j++)
            {
                float theta = j * thetaStep;
                VertexType::ColorVertex v = {};

                v.Position.x = _radius * sinf(phi) * cosf(theta);
                v.Position.y = _radius * cosf(phi);
                v.Position.z = _radius * sinf(phi) * sinf(theta);

                XMVECTOR P = XMLoadFloat3(&v.Position);
                XMStoreFloat3(&v.Normal, XMVector3Normalize(P));

                v.Color = _vertColor;

                color.emplace_back(v);
            }
        }

        color.emplace_back(bottomVertex);

        for (UINT i = 1; i <= _sliceCount; i++)
        {
            indeices.emplace_back(0);
            indeices.emplace_back(i + 1);
            indeices.emplace_back(i);
        }

        UINT baseIndex = 1;
        UINT ringVertexCount = _sliceCount + 1;
        for (UINT i = 0; i < _stackCount - 2; i++)
        {
            for (UINT j = 0; j < _sliceCount; j++)
            {
                indeices.emplace_back(
                    baseIndex + i * ringVertexCount + j);
                indeices.emplace_back(
                    baseIndex + i * ringVertexCount + j + 1);
                indeices.emplace_back(
                    baseIndex + (i + 1) * ringVertexCount + j);

                indeices.emplace_back(
                    baseIndex + (i + 1) * ringVertexCount + j);
                indeices.emplace_back(
                    baseIndex + i * ringVertexCount + j + 1);
                indeices.emplace_back(
                    baseIndex + (i + 1) * ringVertexCount + j + 1);
            }
        }

        UINT southPoleIndex = (UINT)color.size() - 1;
        baseIndex = southPoleIndex - ringVertexCount;

        for (UINT i = 0; i < _sliceCount; i++)
        {
            indeices.emplace_back(southPoleIndex);
            indeices.emplace_back(baseIndex + i);
            indeices.emplace_back(baseIndex + i + 1);
        }

        si.mTopologyType = TOPOLOGY_TYPE::TRIANGLELIST;
        si.mVerteices = &color;
        si.mIndeices = &indeices;
        si.mTextures = &textures;
        si.mMaterial = &mi;
        mMeshHelperPtr->ProcessSubMesh(&rsd, &si, _layout);

        break;
    }

    case LAYOUT_TYPE::NORMAL_TEX:
    {
        if (_useVertexColor)
        {
            bool usingVertColor = false;
            assert(usingVertColor);
        }
        VertexType::BasicVertex topVertex =
        {
            { 0.0f, +_radius, 0.0f }, { 0.0f, +1.0f, 0.0f },
            { 0.0f, 0.0f }
        };
        VertexType::BasicVertex bottomVertex =
        {
            { 0.0f, -_radius, 0.0f }, { 0.0f, -1.0f, 0.0f },
            { 0.0f, 1.0f }
        };

        basic.emplace_back(topVertex);

        float phiStep = DirectX::XM_PI / _stackCount;
        float thetaStep = 2.0f * DirectX::XM_PI / _sliceCount;

        for (UINT i = 1; i <= _stackCount - 1; i++)
        {
            float phi = i * phiStep;

            for (UINT j = 0; j <= _sliceCount; j++)
            {
                float theta = j * thetaStep;
                VertexType::BasicVertex v = {};

                v.Position.x = _radius * sinf(phi) * cosf(theta);
                v.Position.y = _radius * cosf(phi);
                v.Position.z = _radius * sinf(phi) * sinf(theta);

                XMVECTOR P = XMLoadFloat3(&v.Position);
                XMStoreFloat3(&v.Normal, XMVector3Normalize(P));

                v.TexCoord.x = theta / DirectX::XM_2PI;
                v.TexCoord.y = phi / DirectX::XM_PI;

                basic.emplace_back(v);
            }
        }

        basic.emplace_back(bottomVertex);

        for (UINT i = 1; i <= _sliceCount; i++)
        {
            indeices.emplace_back(0);
            indeices.emplace_back(i + 1);
            indeices.emplace_back(i);
        }

        UINT baseIndex = 1;
        UINT ringVertexCount = _sliceCount + 1;
        for (UINT i = 0; i < _stackCount - 2; i++)
        {
            for (UINT j = 0; j < _sliceCount; j++)
            {
                indeices.emplace_back(
                    baseIndex + i * ringVertexCount + j);
                indeices.emplace_back(
                    baseIndex + i * ringVertexCount + j + 1);
                indeices.emplace_back(
                    baseIndex + (i + 1) * ringVertexCount + j);

                indeices.emplace_back(
                    baseIndex + (i + 1) * ringVertexCount + j);
                indeices.emplace_back(
                    baseIndex + i * ringVertexCount + j + 1);
                indeices.emplace_back(
                    baseIndex + (i + 1) * ringVertexCount + j + 1);
            }
        }

        UINT southPoleIndex = (UINT)basic.size() - 1;
        baseIndex = southPoleIndex - ringVertexCount;

        for (UINT i = 0; i < _sliceCount; i++)
        {
            indeices.emplace_back(southPoleIndex);
            indeices.emplace_back(baseIndex + i);
            indeices.emplace_back(baseIndex + i + 1);
        }

        textures.emplace_back(_texColorName);

        si.mTopologyType = TOPOLOGY_TYPE::TRIANGLELIST;
        si.mVerteices = &basic;
        si.mIndeices = &indeices;
        si.mTextures = &textures;
        si.mMaterial = &mi;
        mMeshHelperPtr->ProcessSubMesh(&rsd, &si, _layout);

        break;
    }

    case LAYOUT_TYPE::NORMAL_TANGENT_TEX:
    {
        if (_useVertexColor)
        {
            bool usingVertColor = false;
            assert(usingVertColor);
        }
        VertexType::TangentVertex topVertex =
        {
            { 0.0f, +_radius, 0.0f }, { 0.0f, +1.0f, 0.0f },
            { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }
        };
        VertexType::TangentVertex bottomVertex =
        {
            { 0.0f, -_radius, 0.0f }, { 0.0f, -1.0f, 0.0f },
            { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f }
        };

        tangent.emplace_back(topVertex);

        float phiStep = DirectX::XM_PI / _stackCount;
        float thetaStep = 2.0f * DirectX::XM_PI / _sliceCount;

        for (UINT i = 1; i <= _stackCount - 1; i++)
        {
            float phi = i * phiStep;

            for (UINT j = 0; j <= _sliceCount; j++)
            {
                float theta = j * thetaStep;
                VertexType::TangentVertex v = {};

                v.Position.x = _radius * sinf(phi) * cosf(theta);
                v.Position.y = _radius * cosf(phi);
                v.Position.z = _radius * sinf(phi) * sinf(theta);
                v.Tangent.x = -_radius * sinf(phi) * sinf(theta);
                v.Tangent.y = 0.0f;
                v.Tangent.z = +_radius * sinf(phi) * cosf(theta);

                XMVECTOR T = XMLoadFloat3(&v.Tangent);
                XMStoreFloat3(&v.Tangent, XMVector3Normalize(T));
                XMVECTOR P = XMLoadFloat3(&v.Position);
                XMStoreFloat3(&v.Normal, XMVector3Normalize(P));

                v.TexCoord.x = theta / DirectX::XM_2PI;
                v.TexCoord.y = phi / DirectX::XM_PI;

                tangent.emplace_back(v);
            }
        }

        tangent.emplace_back(bottomVertex);

        for (UINT i = 1; i <= _sliceCount; i++)
        {
            indeices.emplace_back(0);
            indeices.emplace_back(i + 1);
            indeices.emplace_back(i);
        }

        UINT baseIndex = 1;
        UINT ringVertexCount = _sliceCount + 1;
        for (UINT i = 0; i < _stackCount - 2; i++)
        {
            for (UINT j = 0; j < _sliceCount; j++)
            {
                indeices.emplace_back(
                    baseIndex + i * ringVertexCount + j);
                indeices.emplace_back(
                    baseIndex + i * ringVertexCount + j + 1);
                indeices.emplace_back(
                    baseIndex + (i + 1) * ringVertexCount + j);

                indeices.emplace_back(
                    baseIndex + (i + 1) * ringVertexCount + j);
                indeices.emplace_back(
                    baseIndex + i * ringVertexCount + j + 1);
                indeices.emplace_back(
                    baseIndex + (i + 1) * ringVertexCount + j + 1);
            }
        }

        UINT southPoleIndex = (UINT)tangent.size() - 1;
        baseIndex = southPoleIndex - ringVertexCount;

        for (UINT i = 0; i < _sliceCount; i++)
        {
            indeices.emplace_back(southPoleIndex);
            indeices.emplace_back(baseIndex + i);
            indeices.emplace_back(baseIndex + i + 1);
        }

        textures.emplace_back(_texColorName);

        si.mTopologyType = TOPOLOGY_TYPE::TRIANGLELIST;
        si.mVerteices = &tangent;
        si.mIndeices = &indeices;
        si.mTextures = &textures;
        si.mMaterial = &mi;
        mMeshHelperPtr->ProcessSubMesh(&rsd, &si, _layout);

        break;
    }

    default:
        bool nullLayout = false;
        assert(nullLayout);
    }

    return rsd;
}

RS_SUBMESH_DATA RSGeometryGenerator::CreateGeometrySphere(
    float _radius, UINT _diviNum,
    LAYOUT_TYPE _layout, bool _useVertexColor,
    DirectX::XMFLOAT4&& _vertColor, std::string&& _texColorName)
{
    RS_SUBMESH_DATA rsd = {};
    SUBMESH_INFO si = {};
    MATERIAL_INFO mi = {};
    std::vector<UINT> indeices = {};
    std::vector<VertexType::BasicVertex> basic = {};
    std::vector<VertexType::TangentVertex> tangent = {};
    std::vector<VertexType::ColorVertex> color = {};
    std::vector<std::string> textures = {};

    switch (_layout)
    {
    case LAYOUT_TYPE::NORMAL_COLOR:
    {
        if (!_useVertexColor)
        {
            bool notUsingVertColor = false;
            assert(notUsingVertColor);
        }

        if (_diviNum > 6) { _diviNum = 6; }

        const float X = 0.525731f;
        const float Z = 0.850651f;
        XMFLOAT3 pos[12] =
        {
            XMFLOAT3(-X, 0.0f, Z),  XMFLOAT3(X, 0.0f, Z),
            XMFLOAT3(-X, 0.0f, -Z), XMFLOAT3(X, 0.0f, -Z),
            XMFLOAT3(0.0f, Z, X),   XMFLOAT3(0.0f, Z, -X),
            XMFLOAT3(0.0f, -Z, X),  XMFLOAT3(0.0f, -Z, -X),
            XMFLOAT3(Z, X, 0.0f),   XMFLOAT3(-Z, X, 0.0f),
            XMFLOAT3(Z, -X, 0.0f),  XMFLOAT3(-Z, -X, 0.0f)
        };
        UINT k[60] =
        {
            1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
            1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
            3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
            10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
        };

        color.resize(ARRAYSIZE(pos));
        indeices.assign(&k[0], &k[60]);

        for (UINT i = 0; i < ARRAYSIZE(pos); i++)
        {
            color[i].Position = pos[i];
        }
        for (UINT i = 0; i < _diviNum; i++)
        {
            SubDivide(_layout, &color, &indeices);
        }

        for (UINT i = 0; i < (UINT)color.size(); ++i)
        {
            XMVECTOR n = XMVector3Normalize(
                XMLoadFloat3(&color[i].Position));
            XMVECTOR p = _radius * n;

            XMStoreFloat3(&color[i].Position, p);
            XMStoreFloat3(&color[i].Normal, n);

            float theta = atan2f(
                color[i].Position.z, color[i].Position.x);

            if (theta < 0.0f)
            {
                theta += DirectX::XM_2PI;
            }

            float phi = acosf(color[i].Position.y / _radius);
            color[i].Color = _vertColor;
        }

        si.mTopologyType = TOPOLOGY_TYPE::TRIANGLELIST;
        si.mVerteices = &color;
        si.mIndeices = &indeices;
        si.mTextures = &textures;
        si.mMaterial = &mi;
        mMeshHelperPtr->ProcessSubMesh(&rsd, &si, _layout);

        break;
    }

    case LAYOUT_TYPE::NORMAL_TEX:
    {
        if (_useVertexColor)
        {
            bool usingVertColor = false;
            assert(usingVertColor);
        }

        if (_diviNum > 6) { _diviNum = 6; }

        const float X = 0.525731f;
        const float Z = 0.850651f;
        XMFLOAT3 pos[12] =
        {
            XMFLOAT3(-X, 0.0f, Z),  XMFLOAT3(X, 0.0f, Z),
            XMFLOAT3(-X, 0.0f, -Z), XMFLOAT3(X, 0.0f, -Z),
            XMFLOAT3(0.0f, Z, X),   XMFLOAT3(0.0f, Z, -X),
            XMFLOAT3(0.0f, -Z, X),  XMFLOAT3(0.0f, -Z, -X),
            XMFLOAT3(Z, X, 0.0f),   XMFLOAT3(-Z, X, 0.0f),
            XMFLOAT3(Z, -X, 0.0f),  XMFLOAT3(-Z, -X, 0.0f)
        };
        UINT k[60] =
        {
            1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
            1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
            3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
            10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
        };

        basic.resize(ARRAYSIZE(pos));
        indeices.assign(&k[0], &k[60]);

        for (UINT i = 0; i < ARRAYSIZE(pos); i++)
        {
            basic[i].Position = pos[i];
        }
        for (UINT i = 0; i < _diviNum; i++)
        {
            SubDivide(_layout, &basic, &indeices);
        }

        for (UINT i = 0; i < (UINT)basic.size(); ++i)
        {
            XMVECTOR n = XMVector3Normalize(
                XMLoadFloat3(&basic[i].Position));
            XMVECTOR p = _radius * n;

            XMStoreFloat3(&basic[i].Position, p);
            XMStoreFloat3(&basic[i].Normal, n);

            float theta = atan2f(
                basic[i].Position.z, basic[i].Position.x);

            if (theta < 0.0f)
            {
                theta += DirectX::XM_2PI;
            }

            float phi = acosf(basic[i].Position.y / _radius);

            basic[i].TexCoord.x = theta / DirectX::XM_2PI;
            basic[i].TexCoord.y = phi / DirectX::XM_PI;
        }

        textures.emplace_back(_texColorName);

        si.mTopologyType = TOPOLOGY_TYPE::TRIANGLELIST;
        si.mVerteices = &basic;
        si.mIndeices = &indeices;
        si.mTextures = &textures;
        si.mMaterial = &mi;
        mMeshHelperPtr->ProcessSubMesh(&rsd, &si, _layout);

        break;
    }

    case LAYOUT_TYPE::NORMAL_TANGENT_TEX:
    {
        if (_useVertexColor)
        {
            bool usingVertColor = false;
            assert(usingVertColor);
        }

        if (_diviNum > 6) { _diviNum = 6; }

        const float X = 0.525731f;
        const float Z = 0.850651f;
        XMFLOAT3 pos[12] =
        {
            XMFLOAT3(-X, 0.0f, Z),  XMFLOAT3(X, 0.0f, Z),
            XMFLOAT3(-X, 0.0f, -Z), XMFLOAT3(X, 0.0f, -Z),
            XMFLOAT3(0.0f, Z, X),   XMFLOAT3(0.0f, Z, -X),
            XMFLOAT3(0.0f, -Z, X),  XMFLOAT3(0.0f, -Z, -X),
            XMFLOAT3(Z, X, 0.0f),   XMFLOAT3(-Z, X, 0.0f),
            XMFLOAT3(Z, -X, 0.0f),  XMFLOAT3(-Z, -X, 0.0f)
        };
        UINT k[60] =
        {
            1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
            1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
            3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
            10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
        };

        tangent.resize(ARRAYSIZE(pos));
        indeices.assign(&k[0], &k[60]);

        for (UINT i = 0; i < ARRAYSIZE(pos); i++)
        {
            tangent[i].Position = pos[i];
        }
        for (UINT i = 0; i < _diviNum; i++)
        {
            SubDivide(_layout, &tangent, &indeices);
        }

        for (UINT i = 0; i < (UINT)tangent.size(); ++i)
        {
            XMVECTOR n = XMVector3Normalize(
                XMLoadFloat3(&tangent[i].Position));
            XMVECTOR p = _radius * n;

            XMStoreFloat3(&tangent[i].Position, p);
            XMStoreFloat3(&tangent[i].Normal, n);

            float theta = atan2f(
                tangent[i].Position.z, tangent[i].Position.x);

            if (theta < 0.0f)
            {
                theta += DirectX::XM_2PI;
            }

            float phi = acosf(tangent[i].Position.y / _radius);

            tangent[i].TexCoord.x = theta / DirectX::XM_2PI;
            tangent[i].TexCoord.y = phi / DirectX::XM_PI;

            tangent[i].Tangent.x = -_radius * sinf(phi) * sinf(theta);
            tangent[i].Tangent.y = 0.0f;
            tangent[i].Tangent.z = +_radius * sinf(phi) * cosf(theta);

            XMVECTOR T = XMLoadFloat3(&tangent[i].Tangent);
            XMStoreFloat3(&tangent[i].Tangent, XMVector3Normalize(T));
        }

        textures.emplace_back(_texColorName);

        si.mTopologyType = TOPOLOGY_TYPE::TRIANGLELIST;
        si.mVerteices = &tangent;
        si.mIndeices = &indeices;
        si.mTextures = &textures;
        si.mMaterial = &mi;
        mMeshHelperPtr->ProcessSubMesh(&rsd, &si, _layout);

        break;
    }

    default:
        bool nullLayout = false;
        assert(nullLayout);
    }

    return rsd;
}

RS_SUBMESH_DATA RSGeometryGenerator::CreateCylinder(
    float _bottomRadius, float _topRadius, float _height,
    UINT _sliceCount, UINT _stackCount,
    LAYOUT_TYPE _layout, bool _useVertexColor,
    DirectX::XMFLOAT4&& _vertColor, std::string&& _texColorName)
{
    RS_SUBMESH_DATA rsd = {};
    SUBMESH_INFO si = {};
    MATERIAL_INFO mi = {};
    std::vector<UINT> indeices = {};
    std::vector<VertexType::BasicVertex> basic = {};
    std::vector<VertexType::TangentVertex> tangent = {};
    std::vector<VertexType::ColorVertex> color = {};
    std::vector<std::string> textures = {};

    switch (_layout)
    {
    case LAYOUT_TYPE::NORMAL_COLOR:
    {
        float stackHeight = _height / _stackCount;
        float radiusStep = (_topRadius - _bottomRadius) / _stackCount;
        UINT ringCount = _stackCount + 1;

        for (UINT i = 0; i < ringCount; i++)
        {
            float y = -0.5f * _height + i * stackHeight;
            float r = _bottomRadius + i * radiusStep;
            float dTheta = 2.0f * DirectX::XM_PI / _sliceCount;

            for (UINT j = 0; j <= _sliceCount; j++)
            {
                VertexType::ColorVertex vertex = {};

                float c = cosf(j * dTheta);
                float s = sinf(j * dTheta);

                vertex.Color = _vertColor;
                vertex.Position =
                    DirectX::XMFLOAT3(r * c, y, r * s);
                DirectX::XMFLOAT3 tangentV =
                    DirectX::XMFLOAT3(-s, 0.f, c);

                float dr = _bottomRadius - _topRadius;
                XMFLOAT3 bitangent(dr * c, -_height, dr * s);
                XMVECTOR T = DirectX::XMLoadFloat3(&tangentV);
                XMVECTOR B = DirectX::XMLoadFloat3(&bitangent);
                XMVECTOR N = DirectX::XMVector3Normalize(
                    DirectX::XMVector3Cross(T, B));
                DirectX::XMStoreFloat3(&vertex.Normal, N);

                color.emplace_back(vertex);
            }
        }

        UINT ringVertexCount = _sliceCount + 1;

        for (UINT i = 0; i < _stackCount; ++i)
        {
            for (UINT j = 0; j < _sliceCount; ++j)
            {
                indeices.emplace_back(i * ringVertexCount + j);
                indeices.emplace_back((i + 1) * ringVertexCount + j);
                indeices.emplace_back((i + 1) * ringVertexCount + j + 1);

                indeices.emplace_back(i * ringVertexCount + j);
                indeices.emplace_back((i + 1) * ringVertexCount + j + 1);
                indeices.emplace_back(i * ringVertexCount + j + 1);
            }
        }

        BuildCylinderTopCap(_bottomRadius, _topRadius, _height,
            _sliceCount, _stackCount, _layout,
            &color, &indeices, _vertColor);
        BuildCylinderBottomCap(_bottomRadius, _topRadius, _height,
            _sliceCount, _stackCount, _layout,
            &color, &indeices, _vertColor);

        si.mTopologyType = TOPOLOGY_TYPE::TRIANGLELIST;
        si.mVerteices = &color;
        si.mIndeices = &indeices;
        si.mTextures = &textures;
        si.mMaterial = &mi;
        mMeshHelperPtr->ProcessSubMesh(&rsd, &si, _layout);

        break;
    }

    case LAYOUT_TYPE::NORMAL_TEX:
    {
        float stackHeight = _height / _stackCount;
        float radiusStep = (_topRadius - _bottomRadius) / _stackCount;
        UINT ringCount = _stackCount + 1;

        for (UINT i = 0; i < ringCount; i++)
        {
            float y = -0.5f * _height + i * stackHeight;
            float r = _bottomRadius + i * radiusStep;
            float dTheta = 2.0f * DirectX::XM_PI / _sliceCount;

            for (UINT j = 0; j <= _sliceCount; j++)
            {
                VertexType::BasicVertex vertex = {};

                float c = cosf(j * dTheta);
                float s = sinf(j * dTheta);

                vertex.Position =
                    DirectX::XMFLOAT3(r * c, y, r * s);
                vertex.TexCoord.x = (float)j / _sliceCount;
                vertex.TexCoord.y = 1.0f - (float)i / _stackCount;
                DirectX::XMFLOAT3 tangentV =
                    DirectX::XMFLOAT3(-s, 0.f, c);

                float dr = _bottomRadius - _topRadius;
                XMFLOAT3 bitangent(dr * c, -_height, dr * s);
                XMVECTOR T = DirectX::XMLoadFloat3(&tangentV);
                XMVECTOR B = DirectX::XMLoadFloat3(&bitangent);
                XMVECTOR N = DirectX::XMVector3Normalize(
                    DirectX::XMVector3Cross(T, B));
                DirectX::XMStoreFloat3(&vertex.Normal, N);

                basic.emplace_back(vertex);
            }
        }

        UINT ringVertexCount = _sliceCount + 1;

        for (UINT i = 0; i < _stackCount; ++i)
        {
            for (UINT j = 0; j < _sliceCount; ++j)
            {
                indeices.emplace_back(i * ringVertexCount + j);
                indeices.emplace_back((i + 1) * ringVertexCount + j);
                indeices.emplace_back((i + 1) * ringVertexCount + j + 1);

                indeices.emplace_back(i * ringVertexCount + j);
                indeices.emplace_back((i + 1) * ringVertexCount + j + 1);
                indeices.emplace_back(i * ringVertexCount + j + 1);
            }
        }

        BuildCylinderTopCap(_bottomRadius, _topRadius, _height,
            _sliceCount, _stackCount, _layout,
            &basic, &indeices, _vertColor);
        BuildCylinderBottomCap(_bottomRadius, _topRadius, _height,
            _sliceCount, _stackCount, _layout,
            &basic, &indeices, _vertColor);

        textures.emplace_back(_texColorName);

        si.mTopologyType = TOPOLOGY_TYPE::TRIANGLELIST;
        si.mVerteices = &basic;
        si.mIndeices = &indeices;
        si.mTextures = &textures;
        si.mMaterial = &mi;
        mMeshHelperPtr->ProcessSubMesh(&rsd, &si, _layout);

        break;
    }

    case LAYOUT_TYPE::NORMAL_TANGENT_TEX:
    {
        float stackHeight = _height / _stackCount;
        float radiusStep = (_topRadius - _bottomRadius) / _stackCount;
        UINT ringCount = _stackCount + 1;

        for (UINT i = 0; i < ringCount; i++)
        {
            float y = -0.5f * _height + i * stackHeight;
            float r = _bottomRadius + i * radiusStep;
            float dTheta = 2.0f * DirectX::XM_PI / _sliceCount;

            for (UINT j = 0; j <= _sliceCount; j++)
            {
                VertexType::TangentVertex vertex = {};

                float c = cosf(j * dTheta);
                float s = sinf(j * dTheta);

                vertex.Position = DirectX::XMFLOAT3(r * c, y, r * s);
                vertex.TexCoord.x = (float)j / _sliceCount;
                vertex.TexCoord.y = 1.0f - (float)i / _stackCount;
                vertex.Tangent = DirectX::XMFLOAT3(-s, 0.f, c);

                float dr = _bottomRadius - _topRadius;
                XMFLOAT3 bitangent(dr * c, -_height, dr * s);
                XMVECTOR T = DirectX::XMLoadFloat3(&vertex.Tangent);
                XMVECTOR B = DirectX::XMLoadFloat3(&bitangent);
                XMVECTOR N = DirectX::XMVector3Normalize(
                    DirectX::XMVector3Cross(T, B));
                DirectX::XMStoreFloat3(&vertex.Normal, N);

                tangent.emplace_back(vertex);
            }
        }

        UINT ringVertexCount = _sliceCount + 1;

        for (UINT i = 0; i < _stackCount; ++i)
        {
            for (UINT j = 0; j < _sliceCount; ++j)
            {
                indeices.emplace_back(i * ringVertexCount + j);
                indeices.emplace_back((i + 1) * ringVertexCount + j);
                indeices.emplace_back((i + 1) * ringVertexCount + j + 1);

                indeices.emplace_back(i * ringVertexCount + j);
                indeices.emplace_back((i + 1) * ringVertexCount + j + 1);
                indeices.emplace_back(i * ringVertexCount + j + 1);
            }
        }

        BuildCylinderTopCap(_bottomRadius, _topRadius, _height,
            _sliceCount, _stackCount, _layout,
            &tangent, &indeices, _vertColor);
        BuildCylinderBottomCap(_bottomRadius, _topRadius, _height,
            _sliceCount, _stackCount, _layout,
            &tangent, &indeices, _vertColor);

        textures.emplace_back(_texColorName);

        si.mTopologyType = TOPOLOGY_TYPE::TRIANGLELIST;
        si.mVerteices = &tangent;
        si.mIndeices = &indeices;
        si.mTextures = &textures;
        si.mMaterial = &mi;
        mMeshHelperPtr->ProcessSubMesh(&rsd, &si, _layout);

        break;
    }

    default:
        bool nullLayout = false;
        assert(nullLayout);
    }

    return rsd;
}

RS_SUBMESH_DATA RSGeometryGenerator::CreateGrid(
    float _width, float _depth, UINT _rowCount, UINT _colCount,
    LAYOUT_TYPE _layout, bool _useVertexColor,
    DirectX::XMFLOAT4&& _vertColor, std::string&& _texColorName)
{
    RS_SUBMESH_DATA rsd = {};
    SUBMESH_INFO si = {};
    MATERIAL_INFO mi = {};
    std::vector<UINT> indeices = {};
    std::vector<VertexType::BasicVertex> basic = {};
    std::vector<VertexType::TangentVertex> tangent = {};
    std::vector<VertexType::ColorVertex> color = {};
    std::vector<std::string> textures = {};

    switch (_layout)
    {
    case LAYOUT_TYPE::NORMAL_COLOR:
    {
        UINT vertexCount = _rowCount * _colCount;
        UINT faceCount = (_rowCount - 1) * (_colCount - 1) * 2;
        float halfWidth = 0.5f * _width;
        float halfDepth = 0.5f * _depth;
        float dx = _width / (_colCount - 1);
        float dz = _depth / (_rowCount - 1);
        float du = 1.f / (_colCount - 1);
        float dv = 1.f / (_rowCount - 1);

        color.resize(vertexCount);
        for (UINT i = 0; i < _rowCount; i++)
        {
            float z = halfDepth - i * dz;
            for (UINT j = 0; j < _colCount; j++)
            {
                float x = -halfWidth + j * dx;

                color[static_cast<std::vector<VertexType::ColorVertex, std::allocator<VertexType::ColorVertex>>::size_type>(i) * _colCount + j].Position =
                    DirectX::XMFLOAT3(x, 0.f, z);
                color[static_cast<std::vector<VertexType::ColorVertex, std::allocator<VertexType::ColorVertex>>::size_type>(i) * _colCount + j].Normal =
                    DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
                color[static_cast<std::vector<VertexType::ColorVertex, std::allocator<VertexType::ColorVertex>>::size_type>(i) * _colCount + j].Color = _vertColor;
            }
        }

        indeices.resize(static_cast<std::vector<UINT, std::allocator<UINT>>::size_type>(faceCount) * 3);

        UINT k = 0;
        for (UINT i = 0; i < _rowCount - 1; i++)
        {
            for (UINT j = 0; j < _colCount - 1; j++)
            {
                indeices[static_cast<std::vector<UINT, std::allocator<UINT>>::size_type>(k)] = i * _colCount + j;
                indeices[static_cast<std::vector<UINT, std::allocator<UINT>>::size_type>(k) + 1] = i * _colCount + j + 1;
                indeices[static_cast<std::vector<UINT, std::allocator<UINT>>::size_type>(k) + 2] = (i + 1) * _colCount + j;

                indeices[static_cast<std::vector<UINT, std::allocator<UINT>>::size_type>(k) + 3] = (i + 1) * _colCount + j;
                indeices[static_cast<std::vector<UINT, std::allocator<UINT>>::size_type>(k) + 4] = i * _colCount + j + 1;
                indeices[static_cast<std::vector<UINT, std::allocator<UINT>>::size_type>(k) + 5] = (i + 1) * _colCount + j + 1;

                k += 6;
            }
        }

        si.mTopologyType = TOPOLOGY_TYPE::TRIANGLELIST;
        si.mVerteices = &color;
        si.mIndeices = &indeices;
        si.mTextures = &textures;
        si.mMaterial = &mi;
        mMeshHelperPtr->ProcessSubMesh(&rsd, &si, _layout);

        break;
    }

    case LAYOUT_TYPE::NORMAL_TEX:
    {
        UINT vertexCount = _rowCount * _colCount;
        UINT faceCount = (_rowCount - 1) * (_colCount - 1) * 2;
        float halfWidth = 0.5f * _width;
        float halfDepth = 0.5f * _depth;
        float dx = _width / (_colCount - 1);
        float dz = _depth / (_rowCount - 1);
        float du = 1.f / (_colCount - 1);
        float dv = 1.f / (_rowCount - 1);

        basic.resize(vertexCount);
        for (UINT i = 0; i < _rowCount; i++)
        {
            float z = halfDepth - i * dz;
            for (UINT j = 0; j < _colCount; j++)
            {
                float x = -halfWidth + j * dx;

                basic[static_cast<std::vector<VertexType::BasicVertex, std::allocator<VertexType::BasicVertex>>::size_type>(i) * _colCount + j].Position =
                    DirectX::XMFLOAT3(x, 0.f, z);
                basic[static_cast<std::vector<VertexType::BasicVertex, std::allocator<VertexType::BasicVertex>>::size_type>(i) * _colCount + j].Normal =
                    DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
                basic[static_cast<std::vector<VertexType::BasicVertex, std::allocator<VertexType::BasicVertex>>::size_type>(i) * _colCount + j].TexCoord.x = j * du;
                basic[static_cast<std::vector<VertexType::BasicVertex, std::allocator<VertexType::BasicVertex>>::size_type>(i) * _colCount + j].TexCoord.y = i * dv;
            }
        }

        indeices.resize(static_cast<std::vector<UINT, std::allocator<UINT>>::size_type>(faceCount) * 3);

        UINT k = 0;
        for (UINT i = 0; i < _rowCount - 1; i++)
        {
            for (UINT j = 0; j < _colCount - 1; j++)
            {
                indeices[static_cast<std::vector<UINT, std::allocator<UINT>>::size_type>(k)] = i * _colCount + j;
                indeices[static_cast<std::vector<UINT, std::allocator<UINT>>::size_type>(k) + 1] = i * _colCount + j + 1;
                indeices[static_cast<std::vector<UINT, std::allocator<UINT>>::size_type>(k) + 2] = (i + 1) * _colCount + j;

                indeices[static_cast<std::vector<UINT, std::allocator<UINT>>::size_type>(k) + 3] = (i + 1) * _colCount + j;
                indeices[static_cast<std::vector<UINT, std::allocator<UINT>>::size_type>(k) + 4] = i * _colCount + j + 1;
                indeices[static_cast<std::vector<UINT, std::allocator<UINT>>::size_type>(k) + 5] = (i + 1) * _colCount + j + 1;

                k += 6;
            }
        }

        textures.emplace_back(_texColorName);

        si.mTopologyType = TOPOLOGY_TYPE::TRIANGLELIST;
        si.mVerteices = &basic;
        si.mIndeices = &indeices;
        si.mTextures = &textures;
        si.mMaterial = &mi;
        mMeshHelperPtr->ProcessSubMesh(&rsd, &si, _layout);

        break;
    }

    case LAYOUT_TYPE::NORMAL_TANGENT_TEX:
    {
        UINT vertexCount = _rowCount * _colCount;
        UINT faceCount = (_rowCount - 1) * (_colCount - 1) * 2;
        float halfWidth = 0.5f * _width;
        float halfDepth = 0.5f * _depth;
        float dx = _width / (_colCount - 1);
        float dz = _depth / (_rowCount - 1);
        float du = 1.f / (_colCount - 1);
        float dv = 1.f / (_rowCount - 1);

        tangent.resize(vertexCount);
        for (UINT i = 0; i < _rowCount; i++)
        {
            float z = halfDepth - i * dz;
            for (UINT j = 0; j < _colCount; j++)
            {
                float x = -halfWidth + j * dx;

                tangent[static_cast<std::vector<VertexType::TangentVertex, std::allocator<VertexType::TangentVertex>>::size_type>(i) * _colCount + j].Position =
                    DirectX::XMFLOAT3(x, 0.f, z);
                tangent[static_cast<std::vector<VertexType::TangentVertex, std::allocator<VertexType::TangentVertex>>::size_type>(i) * _colCount + j].Normal =
                    DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
                tangent[static_cast<std::vector<VertexType::TangentVertex, std::allocator<VertexType::TangentVertex>>::size_type>(i) * _colCount + j].Tangent =
                    DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);

                tangent[static_cast<std::vector<VertexType::TangentVertex, std::allocator<VertexType::TangentVertex>>::size_type>(i) * _colCount + j].TexCoord.x = j * du;
                tangent[static_cast<std::vector<VertexType::TangentVertex, std::allocator<VertexType::TangentVertex>>::size_type>(i) * _colCount + j].TexCoord.y = i * dv;
            }
        }

        indeices.resize(static_cast<std::vector<UINT, std::allocator<UINT>>::size_type>(faceCount) * 3);

        UINT k = 0;
        for (UINT i = 0; i < _rowCount - 1; i++)
        {
            for (UINT j = 0; j < _colCount - 1; j++)
            {
                indeices[static_cast<std::vector<UINT, std::allocator<UINT>>::size_type>(k)] = i * _colCount + j;
                indeices[static_cast<std::vector<UINT, std::allocator<UINT>>::size_type>(k) + 1] = i * _colCount + j + 1;
                indeices[static_cast<std::vector<UINT, std::allocator<UINT>>::size_type>(k) + 2] = (i + 1) * _colCount + j;

                indeices[static_cast<std::vector<UINT, std::allocator<UINT>>::size_type>(k) + 3] = (i + 1) * _colCount + j;
                indeices[static_cast<std::vector<UINT, std::allocator<UINT>>::size_type>(k) + 4] = i * _colCount + j + 1;
                indeices[static_cast<std::vector<UINT, std::allocator<UINT>>::size_type>(k) + 5] = (i + 1) * _colCount + j + 1;

                k += 6;
            }
        }

        textures.emplace_back(_texColorName);

        si.mTopologyType = TOPOLOGY_TYPE::TRIANGLELIST;
        si.mVerteices = &tangent;
        si.mIndeices = &indeices;
        si.mTextures = &textures;
        si.mMaterial = &mi;
        mMeshHelperPtr->ProcessSubMesh(&rsd, &si, _layout);

        break;
    }

    default:
        bool nullLayout = false;
        assert(nullLayout);
    }

    return rsd;
}

void RSGeometryGenerator::SubDivide(LAYOUT_TYPE _layout,
    void* _vertexVec, std::vector<UINT>* _indexVec)
{
    switch (_layout)
    {
    case LAYOUT_TYPE::NORMAL_COLOR:
    {
        std::vector<VertexType::ColorVertex>* _colorVec =
            (std::vector<VertexType::ColorVertex>*)_vertexVec;
        std::vector<VertexType::ColorVertex> colorCopy = *_colorVec;
        std::vector<UINT> indexCopy = *_indexVec;

        _colorVec->resize(0);
        _indexVec->resize(0);

        UINT numTris = (UINT)indexCopy.size() / 3;
        for (UINT i = 0; i < numTris; i++)
        {
            VertexType::ColorVertex v0 =
                colorCopy[indexCopy[i * 3 + 0]];
            VertexType::ColorVertex v1 =
                colorCopy[indexCopy[i * 3 + 1]];
            VertexType::ColorVertex v2 =
                colorCopy[indexCopy[i * 3 + 2]];

            VertexType::ColorVertex m0 = ColorMidPoint(v0, v1);
            VertexType::ColorVertex m1 = ColorMidPoint(v1, v2);
            VertexType::ColorVertex m2 = ColorMidPoint(v0, v2);

            _colorVec->emplace_back(v0);
            _colorVec->emplace_back(v1);
            _colorVec->emplace_back(v2);
            _colorVec->emplace_back(m0);
            _colorVec->emplace_back(m1);
            _colorVec->emplace_back(m2);
            _indexVec->emplace_back(i * 6 + 0);
            _indexVec->emplace_back(i * 6 + 3);
            _indexVec->emplace_back(i * 6 + 5);
            _indexVec->emplace_back(i * 6 + 3);
            _indexVec->emplace_back(i * 6 + 4);
            _indexVec->emplace_back(i * 6 + 5);
            _indexVec->emplace_back(i * 6 + 5);
            _indexVec->emplace_back(i * 6 + 4);
            _indexVec->emplace_back(i * 6 + 2);
            _indexVec->emplace_back(i * 6 + 3);
            _indexVec->emplace_back(i * 6 + 1);
            _indexVec->emplace_back(i * 6 + 4);
        }

        break;
    }

    case LAYOUT_TYPE::NORMAL_TEX:
    {
        std::vector<VertexType::BasicVertex>* _basicVec =
            (std::vector<VertexType::BasicVertex>*)_vertexVec;
        std::vector<VertexType::BasicVertex> basicCopy = *_basicVec;
        std::vector<UINT> indexCopy = *_indexVec;

        _basicVec->resize(0);
        _indexVec->resize(0);

        UINT numTris = (UINT)indexCopy.size() / 3;
        for (UINT i = 0; i < numTris; i++)
        {
            VertexType::BasicVertex v0 =
                basicCopy[indexCopy[i * 3 + 0]];
            VertexType::BasicVertex v1 =
                basicCopy[indexCopy[i * 3 + 1]];
            VertexType::BasicVertex v2 =
                basicCopy[indexCopy[i * 3 + 2]];

            VertexType::BasicVertex m0 = BasicMidPoint(v0, v1);
            VertexType::BasicVertex m1 = BasicMidPoint(v1, v2);
            VertexType::BasicVertex m2 = BasicMidPoint(v0, v2);

            _basicVec->emplace_back(v0);
            _basicVec->emplace_back(v1);
            _basicVec->emplace_back(v2);
            _basicVec->emplace_back(m0);
            _basicVec->emplace_back(m1);
            _basicVec->emplace_back(m2);
            _indexVec->emplace_back(i * 6 + 0);
            _indexVec->emplace_back(i * 6 + 3);
            _indexVec->emplace_back(i * 6 + 5);
            _indexVec->emplace_back(i * 6 + 3);
            _indexVec->emplace_back(i * 6 + 4);
            _indexVec->emplace_back(i * 6 + 5);
            _indexVec->emplace_back(i * 6 + 5);
            _indexVec->emplace_back(i * 6 + 4);
            _indexVec->emplace_back(i * 6 + 2);
            _indexVec->emplace_back(i * 6 + 3);
            _indexVec->emplace_back(i * 6 + 1);
            _indexVec->emplace_back(i * 6 + 4);
        }

        break;
    }

    case LAYOUT_TYPE::NORMAL_TANGENT_TEX:
    {
        std::vector<VertexType::TangentVertex>* _tangeVec =
            (std::vector<VertexType::TangentVertex>*)_vertexVec;
        std::vector<VertexType::TangentVertex> tangeCopy = *_tangeVec;
        std::vector<UINT> indexCopy = *_indexVec;

        _tangeVec->resize(0);
        _indexVec->resize(0);

        UINT numTris = (UINT)indexCopy.size() / 3;
        for (UINT i = 0; i < numTris; i++)
        {
            VertexType::TangentVertex v0 =
                tangeCopy[indexCopy[i * 3 + 0]];
            VertexType::TangentVertex v1 =
                tangeCopy[indexCopy[i * 3 + 1]];
            VertexType::TangentVertex v2 =
                tangeCopy[indexCopy[i * 3 + 2]];

            VertexType::TangentVertex m0 = TangentMidPoint(v0, v1);
            VertexType::TangentVertex m1 = TangentMidPoint(v1, v2);
            VertexType::TangentVertex m2 = TangentMidPoint(v0, v2);

            _tangeVec->emplace_back(v0);
            _tangeVec->emplace_back(v1);
            _tangeVec->emplace_back(v2);
            _tangeVec->emplace_back(m0);
            _tangeVec->emplace_back(m1);
            _tangeVec->emplace_back(m2);
            _indexVec->emplace_back(i * 6 + 0);
            _indexVec->emplace_back(i * 6 + 3);
            _indexVec->emplace_back(i * 6 + 5);
            _indexVec->emplace_back(i * 6 + 3);
            _indexVec->emplace_back(i * 6 + 4);
            _indexVec->emplace_back(i * 6 + 5);
            _indexVec->emplace_back(i * 6 + 5);
            _indexVec->emplace_back(i * 6 + 4);
            _indexVec->emplace_back(i * 6 + 2);
            _indexVec->emplace_back(i * 6 + 3);
            _indexVec->emplace_back(i * 6 + 1);
            _indexVec->emplace_back(i * 6 + 4);
        }

        break;
    }

    default:
        bool nullLayout = false;
        assert(nullLayout);
    }
}

VertexType::BasicVertex RSGeometryGenerator::BasicMidPoint(
    const VertexType::BasicVertex& _v0,
    const VertexType::BasicVertex& _v1)
{
    DirectX::XMVECTOR p0 = DirectX::XMLoadFloat3(&_v0.Position);
    DirectX::XMVECTOR p1 = DirectX::XMLoadFloat3(&_v1.Position);
    DirectX::XMVECTOR n0 = DirectX::XMLoadFloat3(&_v0.Normal);
    DirectX::XMVECTOR n1 = DirectX::XMLoadFloat3(&_v1.Normal);
    DirectX::XMVECTOR tex0 = DirectX::XMLoadFloat2(&_v0.TexCoord);
    DirectX::XMVECTOR tex1 = DirectX::XMLoadFloat2(&_v1.TexCoord);
    DirectX::XMVECTOR pos = 0.5f * (p0 + p1);
    DirectX::XMVECTOR normal =
        DirectX::XMVector3Normalize(0.5f * (n0 + n1));
    DirectX::XMVECTOR tex = 0.5f * (tex0 + tex1);

    VertexType::BasicVertex v = {};
    DirectX::XMStoreFloat3(&v.Position, pos);
    DirectX::XMStoreFloat3(&v.Normal, normal);
    DirectX::XMStoreFloat2(&v.TexCoord, tex);

    return v;
}

VertexType::TangentVertex RSGeometryGenerator::TangentMidPoint(
    const VertexType::TangentVertex& _v0,
    const VertexType::TangentVertex& _v1)
{
    DirectX::XMVECTOR p0 = DirectX::XMLoadFloat3(&_v0.Position);
    DirectX::XMVECTOR p1 = DirectX::XMLoadFloat3(&_v1.Position);
    DirectX::XMVECTOR n0 = DirectX::XMLoadFloat3(&_v0.Normal);
    DirectX::XMVECTOR n1 = DirectX::XMLoadFloat3(&_v1.Normal);
    DirectX::XMVECTOR tan0 = DirectX::XMLoadFloat3(&_v0.Tangent);
    DirectX::XMVECTOR tan1 = DirectX::XMLoadFloat3(&_v1.Tangent);
    DirectX::XMVECTOR tex0 = DirectX::XMLoadFloat2(&_v0.TexCoord);
    DirectX::XMVECTOR tex1 = DirectX::XMLoadFloat2(&_v1.TexCoord);
    DirectX::XMVECTOR pos = 0.5f * (p0 + p1);
    DirectX::XMVECTOR normal =
        DirectX::XMVector3Normalize(0.5f * (n0 + n1));
    DirectX::XMVECTOR tangent =
        DirectX::XMVector3Normalize(0.5f * (tan0 + tan1));
    DirectX::XMVECTOR tex = 0.5f * (tex0 + tex1);

    VertexType::TangentVertex v = {};
    DirectX::XMStoreFloat3(&v.Position, pos);
    DirectX::XMStoreFloat3(&v.Normal, normal);
    DirectX::XMStoreFloat3(&v.Tangent, tangent);
    DirectX::XMStoreFloat2(&v.TexCoord, tex);

    return v;
}

VertexType::ColorVertex RSGeometryGenerator::ColorMidPoint(
    const VertexType::ColorVertex& _v0,
    const VertexType::ColorVertex& _v1)
{
    DirectX::XMVECTOR p0 = DirectX::XMLoadFloat3(&_v0.Position);
    DirectX::XMVECTOR p1 = DirectX::XMLoadFloat3(&_v1.Position);
    DirectX::XMVECTOR n0 = DirectX::XMLoadFloat3(&_v0.Normal);
    DirectX::XMVECTOR n1 = DirectX::XMLoadFloat3(&_v1.Normal);
    DirectX::XMVECTOR col0 = DirectX::XMLoadFloat4(&_v0.Color);
    DirectX::XMVECTOR col1 = DirectX::XMLoadFloat4(&_v1.Color);
    DirectX::XMVECTOR pos = 0.5f * (p0 + p1);
    DirectX::XMVECTOR color = 0.5f * (col0 + col1);
    DirectX::XMVECTOR normal =
        DirectX::XMVector3Normalize(0.5f * (n0 + n1));

    VertexType::ColorVertex v = {};
    DirectX::XMStoreFloat3(&v.Position, pos);
    DirectX::XMStoreFloat3(&v.Normal, normal);
    DirectX::XMStoreFloat4(&v.Color, color);

    return v;
}

void RSGeometryGenerator::BuildCylinderTopCap(
    float _bottomRadius, float _topRadius, float _height,
    UINT _sliceCount, UINT _stackCount,
    LAYOUT_TYPE _layout, void* _vertexVec,
    std::vector<UINT>* _indexVec,
    DirectX::XMFLOAT4& _vertColor)
{
    switch (_layout)
    {
    case LAYOUT_TYPE::NORMAL_COLOR:
    {
        std::vector<VertexType::ColorVertex>* _colorVec =
            (std::vector<VertexType::ColorVertex>*)_vertexVec;

        UINT baseIndex = (UINT)_colorVec->size();
        float y = 0.5f * _height;
        float dTheta = 2.f * DirectX::XM_PI / _sliceCount;

        for (UINT i = 0; i <= _sliceCount; i++)
        {
            float x = _topRadius * cosf(i * dTheta);
            float z = _topRadius * sinf(i * dTheta);
            float u = x / _height + 0.5f;
            float v = z / _height + 0.5f;

            VertexType::ColorVertex vert =
            {
                { x, y, z }, { 0.0f, 1.0f, 0.0f }, _vertColor
            };
            _colorVec->emplace_back(vert);
        }

        VertexType::ColorVertex vert =
        {
            { 0.0f, y, 0.0f }, { 0.0f, 1.0f, 0.0f }, _vertColor
        };
        _colorVec->emplace_back(vert);

        UINT centerIndex = (UINT)_colorVec->size() - 1;

        for (UINT i = 0; i < _sliceCount; i++)
        {
            _indexVec->emplace_back(centerIndex);
            _indexVec->emplace_back(baseIndex + i + 1);
            _indexVec->emplace_back(baseIndex + i);
        }

        break;
    }

    case LAYOUT_TYPE::NORMAL_TEX:
    {
        std::vector<VertexType::BasicVertex>* _basicVec =
            (std::vector<VertexType::BasicVertex>*)_vertexVec;

        UINT baseIndex = (UINT)_basicVec->size();
        float y = 0.5f * _height;
        float dTheta = 2.f * DirectX::XM_PI / _sliceCount;

        for (UINT i = 0; i <= _sliceCount; i++)
        {
            float x = _topRadius * cosf(i * dTheta);
            float z = _topRadius * sinf(i * dTheta);
            float u = x / _height + 0.5f;
            float v = z / _height + 0.5f;

            VertexType::BasicVertex vert =
            {
                { x, y, z }, { 0.0f, 1.0f, 0.0f }, { u, v }
            };
            _basicVec->emplace_back(vert);
        }

        VertexType::BasicVertex vert =
        {
            { 0.0f, y, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.5f, 0.5f }
        };
        _basicVec->emplace_back(vert);

        UINT centerIndex = (UINT)_basicVec->size() - 1;

        for (UINT i = 0; i < _sliceCount; i++)
        {
            _indexVec->emplace_back(centerIndex);
            _indexVec->emplace_back(baseIndex + i + 1);
            _indexVec->emplace_back(baseIndex + i);
        }

        break;
    }

    case LAYOUT_TYPE::NORMAL_TANGENT_TEX:
    {
        std::vector<VertexType::TangentVertex>* _tangeVec =
            (std::vector<VertexType::TangentVertex>*)_vertexVec;

        UINT baseIndex = (UINT)_tangeVec->size();
        float y = 0.5f * _height;
        float dTheta = 2.f * DirectX::XM_PI / _sliceCount;

        for (UINT i = 0; i <= _sliceCount; i++)
        {
            float x = _topRadius * cosf(i * dTheta);
            float z = _topRadius * sinf(i * dTheta);
            float u = x / _height + 0.5f;
            float v = z / _height + 0.5f;

            VertexType::TangentVertex vert =
            {
                { x, y, z }, { 0.0f, 1.0f, 0.0f },
                { 1.0f, 0.0f, 0.0f }, { u, v }
            };
            _tangeVec->emplace_back(vert);
        }

        VertexType::TangentVertex vert =
        {
            { 0.0f, y, 0.0f }, { 0.0f, 1.0f, 0.0f },
            { 1.0f, 0.0f, 0.0f }, { 0.5f, 0.5f }
        };
        _tangeVec->emplace_back(vert);

        UINT centerIndex = (UINT)_tangeVec->size() - 1;

        for (UINT i = 0; i < _sliceCount; i++)
        {
            _indexVec->emplace_back(centerIndex);
            _indexVec->emplace_back(baseIndex + i + 1);
            _indexVec->emplace_back(baseIndex + i);
        }

        break;
    }

    default:
        bool nullLayout = false;
        assert(nullLayout);
    }
}

void RSGeometryGenerator::BuildCylinderBottomCap(
    float _bottomRadius, float _topRadius, float _height,
    UINT _sliceCount, UINT _stackCount,
    LAYOUT_TYPE _layout, void* _vertexVec,
    std::vector<UINT>* _indexVec,
    DirectX::XMFLOAT4& _vertColor)
{
    switch (_layout)
    {
    case LAYOUT_TYPE::NORMAL_COLOR:
    {
        std::vector<VertexType::ColorVertex>* _colorVec =
            (std::vector<VertexType::ColorVertex>*)_vertexVec;

        UINT baseIndex = (UINT)_colorVec->size();
        float y = -0.5f * _height;
        float dTheta = 2.f * DirectX::XM_PI / _sliceCount;

        for (UINT i = 0; i <= _sliceCount; i++)
        {
            float x = _bottomRadius * cosf(i * dTheta);
            float z = _bottomRadius * sinf(i * dTheta);
            float u = x / _height + 0.5f;
            float v = z / _height + 0.5f;

            VertexType::ColorVertex vert =
            {
                { x, y, z }, { 0.0f, -1.0f, 0.0f }, _vertColor
            };
            _colorVec->emplace_back(vert);
        }

        VertexType::ColorVertex vert =
        {
            { 0.0f, y, 0.0f }, { 0.0f, -1.0f, 0.0f }, _vertColor
        };
        _colorVec->emplace_back(vert);

        UINT centerIndex = (UINT)_colorVec->size() - 1;

        for (UINT i = 0; i < _sliceCount; i++)
        {
            _indexVec->emplace_back(centerIndex);
            _indexVec->emplace_back(baseIndex + i);
            _indexVec->emplace_back(baseIndex + i + 1);
        }

        break;
    }

    case LAYOUT_TYPE::NORMAL_TEX:
    {
        std::vector<VertexType::BasicVertex>* _basicVec =
            (std::vector<VertexType::BasicVertex>*)_vertexVec;

        UINT baseIndex = (UINT)_basicVec->size();
        float y = -0.5f * _height;
        float dTheta = 2.f * DirectX::XM_PI / _sliceCount;

        for (UINT i = 0; i <= _sliceCount; i++)
        {
            float x = _bottomRadius * cosf(i * dTheta);
            float z = _bottomRadius * sinf(i * dTheta);
            float u = x / _height + 0.5f;
            float v = z / _height + 0.5f;

            VertexType::BasicVertex vert =
            {
                { x, y, z }, { 0.0f, -1.0f, 0.0f }, { u, v }
            };
            _basicVec->emplace_back(vert);
        }

        VertexType::BasicVertex vert =
        {
            { 0.0f, y, 0.0f }, { 0.0f, -1.0f, 0.0f }, { 0.5f, 0.5f }
        };
        _basicVec->emplace_back(vert);

        UINT centerIndex = (UINT)_basicVec->size() - 1;

        for (UINT i = 0; i < _sliceCount; i++)
        {
            _indexVec->emplace_back(centerIndex);
            _indexVec->emplace_back(baseIndex + i);
            _indexVec->emplace_back(baseIndex + i + 1);
        }

        break;
    }

    case LAYOUT_TYPE::NORMAL_TANGENT_TEX:
    {
        std::vector<VertexType::TangentVertex>* _tangeVec =
            (std::vector<VertexType::TangentVertex>*)_vertexVec;

        UINT baseIndex = (UINT)_tangeVec->size();
        float y = -0.5f * _height;
        float dTheta = 2.f * DirectX::XM_PI / _sliceCount;

        for (UINT i = 0; i <= _sliceCount; i++)
        {
            float x = _bottomRadius * cosf(i * dTheta);
            float z = _bottomRadius * sinf(i * dTheta);
            float u = x / _height + 0.5f;
            float v = z / _height + 0.5f;

            VertexType::TangentVertex vert =
            {
                { x, y, z }, { 0.0f, -1.0f, 0.0f },
                { 1.0f, 0.0f, 0.0f }, { u, v }
            };
            _tangeVec->emplace_back(vert);
        }

        VertexType::TangentVertex vert =
        {
            { 0.0f, y, 0.0f }, { 0.0f, -1.0f, 0.0f },
            { 1.0f, 0.0f, 0.0f }, { 0.5f, 0.5f }
        };
        _tangeVec->emplace_back(vert);

        UINT centerIndex = (UINT)_tangeVec->size() - 1;

        for (UINT i = 0; i < _sliceCount; i++)
        {
            _indexVec->emplace_back(centerIndex);
            _indexVec->emplace_back(baseIndex + i);
            _indexVec->emplace_back(baseIndex + i + 1);
        }

        break;
    }

    default:
        bool nullLayout = false;
        assert(nullLayout);
    }
}
