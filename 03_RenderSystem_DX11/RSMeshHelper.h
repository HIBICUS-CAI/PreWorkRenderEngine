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

    RS_SUBMESH_DATA ProcessSubMesh(
        SUBMESH_INFO* _info, LAYOUT_TYPE _layoutType);

private:
    ID3D11Buffer* CreateIndexBuffer(
        const std::vector<UINT>* const _indices);
    ID3D11Buffer* CreateVertexBuffer(
        const std::vector<void*>* const _vertices,
        LAYOUT_TYPE _layoutType);
    std::vector<std::string> CreateTexSrv(
        const std::vector<std::string>* const _textures);
    RS_MATERIAL_INFO CreateSubMeshMaterial(
        const MATERIAL_INFO* const _info);
    RS_MATERIAL_INFO RefStaticMaterial(
        std::string& _materialName);

private:
    class RSRoot_DX11* mRootPtr;
    class RSTexturesManager* mTexManagerPtr;
};

