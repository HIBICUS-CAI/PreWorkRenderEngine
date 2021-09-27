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
        class RSTexturesManager* _texManager);
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
    class RSTexturesManager* mTexManagerPtr;
    class RSDevices* mDevicesPtr;
    class RSGeometryGenerator* mGeoGeneratorPtr;
};

class RSGeometryGenerator
{
public:
    RSGeometryGenerator(RSMeshHelper* _helper);
    ~RSGeometryGenerator();

    RS_SUBMESH_DATA CreateBox(
        float _width, float _height, float _depth, UINT _diviNum,
        std::string&& _layoutName, bool _useVertexColor = true,
        DirectX::XMFLOAT4&& _vertColor = { 1.f,1.f,1.f,1.f },
        std::string&& _texColorName = "");

    RS_SUBMESH_DATA CreateSphere(
        float _radius, UINT _sliceCount, UINT _stackCount,
        std::string&& _layoutName, bool _useVertexColor = true,
        DirectX::XMFLOAT4&& _vertColor = { 1.f,1.f,1.f,1.f },
        std::string&& _texColorName = "");

    RS_SUBMESH_DATA CreateGeometrySphere(
        float _radius, UINT _diviNum,
        std::string&& _layoutName, bool _useVertexColor = true,
        DirectX::XMFLOAT4&& _vertColor = { 1.f,1.f,1.f,1.f },
        std::string&& _texColorName = "");

    RS_SUBMESH_DATA CreateCylinder(
        float _bottomRadius, float _topRadius, float _height,
        UINT _sliceCount, UINT _stackCount,
        std::string&& _layoutName, bool _useVertexColor = true,
        DirectX::XMFLOAT4&& _vertColor = { 1.f,1.f,1.f,1.f },
        std::string&& _texColorName = "");

    RS_SUBMESH_DATA CreateGrid(
        float _width, float _depth, UINT _rowCount, UINT _colCount,
        std::string&& _layoutName, bool _useVertexColor = true,
        DirectX::XMFLOAT4&& _vertColor = { 1.f,1.f,1.f,1.f },
        std::string&& _texColorName = "");

private:
    RSMeshHelper* mMeshHelperPtr;
};
