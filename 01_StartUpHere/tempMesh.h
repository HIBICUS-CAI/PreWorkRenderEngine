#pragma once

#pragma warning(disable:26812)

#include <vector>
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <string>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include "tempDeclaration.h"

namespace TEMP
{
    HRESULT PrepareMeshD3D(ID3D11Device* dev,
        HWND wndHandle);

    class SubMesh
    {
    public:
        SubMesh(ID3D11Device* dev,
            std::vector<MESH_VERTEX> vertices,
            std::vector<UINT> indices,
            std::vector<MESH_TEXTURE> textures);

        ~SubMesh();

        void Draw(ID3D11DeviceContext* devContext);

        void DeleteThisSubMesh();

    private:
        bool SetupSubMesh();

        std::vector<MESH_VERTEX> mVertices;
        std::vector<UINT> mIndices;
        std::vector<MESH_TEXTURE> mTextures;
        ID3D11Device* mD3DDev;
        ID3D11Buffer* mVertexBuffer;
        ID3D11Buffer* mIndexBuffer;
    };

    class Mesh
    {
    public:
        Mesh(ID3D11Device* d3DDevice);
        ~Mesh();

        bool Load(std::string fileName);
        void DrawShadowDepth(ID3D11DeviceContext* devContext);
        void Draw(ID3D11DeviceContext* devContext);
        void DrawSsaoNormal(ID3D11DeviceContext* devContext);
        void DeleteThisMesh();

    private:
        void ProcessNode(
            aiNode* node, const aiScene* scene);
        SubMesh ProcessSubMesh(
            aiMesh* mesh, const aiScene* scene);
        std::vector<MESH_TEXTURE> LoadMaterialTextures(
            aiMaterial* mat, aiTextureType type,
            std::string typeName, const aiScene* scene);
        std::string DetermineDiffuseTextureType(
            const aiScene* scene, aiMaterial* mat);
        int GetTextureIndex(aiString* str);
        ID3D11ShaderResourceView* GetTextureFromModel(
            const aiScene* scene, int texIndex);

        ID3D11Device* mD3DDev;
        std::vector<SubMesh> mSubMeshes;
        std::string mDirectory;
        std::vector<MESH_TEXTURE> mLoadedTexs;
        std::string mTextureType;
    };
}
