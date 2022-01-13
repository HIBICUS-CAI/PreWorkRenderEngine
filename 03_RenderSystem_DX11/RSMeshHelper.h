//---------------------------------------------------------------
// File: RSMeshHelper.h
// Proj: RenderSystem_DX11
// Info: 提供对SubMesh转换为DirectX可识别形式的方法
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#pragma once

#include "RSCommon.h"

class RSMeshHelper
{
public:
    RSMeshHelper();
    ~RSMeshHelper();

    bool StartUp(
        class RSRoot_DX11* _root,
        class RSResourceManager* _texManager);
    void CleanAndStop();

    void ProcessSubMesh(RS_SUBMESH_DATA* _result,
        SUBMESH_INFO* _info, LAYOUT_TYPE _layoutType);
    void ReleaseSubMesh(RS_SUBMESH_DATA& _result);

    class RSGeometryGenerator* GeoGenerate();

private:
    ID3D11InputLayout* RefStaticInputLayout(
        LAYOUT_TYPE _layoutType);
    ID3D11Buffer* CreateIndexBuffer(
        const std::vector<UINT>* const _indices);
    ID3D11Buffer* CreateVertexBuffer(
        const void* const _vertices,
        LAYOUT_TYPE _layoutType);
    void CreateTexSrv(RS_SUBMESH_DATA* _result,
        const std::vector<std::string>* const _textures);
    void CreateSubMeshMaterial(RS_SUBMESH_DATA* _result,
        const MATERIAL_INFO* const _info);
    void RefStaticMaterial(RS_SUBMESH_DATA* _result,
        std::string& _materialName);

private:
    class RSRoot_DX11* mRootPtr;
    class RSResourceManager* mTexManagerPtr;
    class RSDevices* mDevicesPtr;
    class RSGeometryGenerator* mGeoGeneratorPtr;
};

class RSGeometryGenerator
{
public:
    RSGeometryGenerator(class RSRoot_DX11* _root);
    ~RSGeometryGenerator();

    RS_SUBMESH_DATA CreateBox(
        float _width, float _height, float _depth, UINT _diviNum,
        LAYOUT_TYPE _layout, bool _useVertexColor = true,
        DirectX::XMFLOAT4&& _vertColor = { 1.f,1.f,1.f,1.f },
        std::string&& _texColorName = "");

    RS_SUBMESH_DATA CreateSphere(
        float _radius, UINT _sliceCount, UINT _stackCount,
        LAYOUT_TYPE _layout, bool _useVertexColor = true,
        DirectX::XMFLOAT4&& _vertColor = { 1.f,1.f,1.f,1.f },
        std::string&& _texColorName = "");

    RS_SUBMESH_DATA CreateGeometrySphere(
        float _radius, UINT _diviNum,
        LAYOUT_TYPE _layout, bool _useVertexColor = true,
        DirectX::XMFLOAT4&& _vertColor = { 1.f,1.f,1.f,1.f },
        std::string&& _texColorName = "");

    RS_SUBMESH_DATA CreateCylinder(
        float _bottomRadius, float _topRadius, float _height,
        UINT _sliceCount, UINT _stackCount,
        LAYOUT_TYPE _layout, bool _useVertexColor = true,
        DirectX::XMFLOAT4&& _vertColor = { 1.f,1.f,1.f,1.f },
        std::string&& _texColorName = "");

    RS_SUBMESH_DATA CreateGrid(
        float _width, float _depth, UINT _rowCount, UINT _colCount,
        LAYOUT_TYPE _layout, bool _useVertexColor = true,
        DirectX::XMFLOAT4&& _vertColor = { 1.f,1.f,1.f,1.f },
        std::string&& _texColorName = "");

    RS_SUBMESH_DATA CreateSpriteRect(
        LAYOUT_TYPE _layout, std::string&& _texPath = "");
    RS_SUBMESH_DATA CreateSpriteRect(
        LAYOUT_TYPE _layout, std::string& _texPath);

private:
    void SubDivide(LAYOUT_TYPE _layout, void* _vertexVec,
        std::vector<UINT>* _indexVec);

    VertexType::BasicVertex BasicMidPoint(
        const VertexType::BasicVertex& _v0,
        const VertexType::BasicVertex& _v1);

    VertexType::TangentVertex TangentMidPoint(
        const VertexType::TangentVertex& _v0,
        const VertexType::TangentVertex& _v1);

    VertexType::ColorVertex ColorMidPoint(
        const VertexType::ColorVertex& _v0,
        const VertexType::ColorVertex& _v1);

    void BuildCylinderTopCap(
        float _bottomRadius, float _topRadius, float _height,
        UINT _sliceCount, UINT _stackCount,
        LAYOUT_TYPE _layout, void* _vertexVec,
        std::vector<UINT>* _indexVec,
        DirectX::XMFLOAT4& _vertColor);

    void BuildCylinderBottomCap(
        float _bottomRadius, float _topRadius, float _height,
        UINT _sliceCount, UINT _stackCount,
        LAYOUT_TYPE _layout, void* _vertexVec,
        std::vector<UINT>* _indexVec,
        DirectX::XMFLOAT4& _vertColor);

private:
    RSMeshHelper* mMeshHelperPtr;
    class RSDevices* mDevicesPtr;
    class RSResourceManager* mTexManagerPtr;
};
