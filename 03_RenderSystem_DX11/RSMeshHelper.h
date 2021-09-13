//---------------------------------------------------------------
// File: RSMeshHelper.h
// Proj: RenderSystem_DX11
// Info: 提供对SubMesh转换为DirectX可识别形式的方法
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include <d3d11_1.h>

// NEED TO MOVE-----------
struct RSMaterialNeedToMove
{
    int a = 0;
};
// NEED TO MOVE-----------

// TEMP-------------
struct SUBMESH_INFO
{
    int a = 0;
};

struct RS_SUBMESH_DATA
{
    int a = 0;
};

struct MATERIAL_INFO
{
    int a = 0;
};

enum class LAYOUT_TYPE
{
    WITH_TANGENT,
    WITHOUT_TANGENT
};
// TEMP-------------

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
        std::vector<UINT>* _indices);
    ID3D11Buffer* CreateVertexBuffer(
        std::vector<void*>* _vertices);
    std::string CreateTexSrv(
        std::vector<std::string>* _textures);
    RSMaterialNeedToMove CreateSubMeshMaterial(
        MATERIAL_INFO* _info);
    RSMaterialNeedToMove RefStaticMaterial(
        std::string& _materialName);

private:
    class RSRoot_DX11* mRootPtr;
    class RSTexturesManager* mTexManagerPtr;
};

