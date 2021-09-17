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
};

