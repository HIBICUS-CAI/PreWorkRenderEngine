#pragma once

#include <vector>
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <string>
#include "tempDeclaration.h"

namespace TEMP
{
    bool PrepareTempMyMesh1(
        std::vector<MESH_VERTEX>* vertices,
        std::vector<UINT>* indices,
        std::vector<MESH_TEXTURE>* textures);
    bool PrepareTempMyMesh2(
        std::vector<MESH_VERTEX>* vertices,
        std::vector<UINT>* indices,
        std::vector<MESH_TEXTURE>* textures);

    ID3D11ShaderResourceView* CreateTextureView(
        const wchar_t* fileName);

    struct WVPConstantBuffer
    {
        DirectX::XMMATRIX mWorld;
        DirectX::XMMATRIX mView;
        DirectX::XMMATRIX mProjection;
        DirectX::XMMATRIX mShadowView;
        DirectX::XMMATRIX mShadowProjection;
        DirectX::XMMATRIX mSsaoVPT;
    };

    class MySubMesh
    {
    public:
        MySubMesh(ID3D11Device* dev, class MyMesh* myMesh,
            std::vector<MESH_VERTEX> vertices,
            std::vector<UINT> indices,
            std::vector<MESH_TEXTURE> textures);

        ~MySubMesh();

        void Draw(ID3D11DeviceContext* devContext);

        void DeleteThisSubMesh();

    private:
        std::vector<MESH_VERTEX> mVertices;
        std::vector<UINT> mIndices;
        std::vector<MESH_TEXTURE> mTextures;
        ID3D11Device* mD3DDev;
        ID3D11Buffer* mVertexBuffer;
        ID3D11Buffer* mIndexBuffer;
        class MyMesh* mMyMesh;
    };

    class MyMesh
    {
    public:
        MyMesh(ID3D11Device* d3DDevice);
        ~MyMesh();

        void CreateSub(
            std::vector<MESH_VERTEX> vertices,
            std::vector<UINT> indices,
            std::vector<MESH_TEXTURE> textures);
        void DrawShadowDepth(ID3D11DeviceContext* devContext);
        void Draw(ID3D11DeviceContext* devContext);
        void DrawSsaoNormal(ID3D11DeviceContext* devContext);
        void DeleteThisMesh();

        ID3D11Buffer* GetWVPBufferPtr()
        {
            return mWVPConstantBuffer;
        }

        void SetPosition(DirectX::XMFLOAT3 pos)
        {
            mPosition = pos;
        }
        void SetScale(DirectX::XMFLOAT3 scale)
        {
            mScale = scale;
        }

    private:
        ID3D11Device* mD3DDev;
        std::vector<MySubMesh> mSubMeshes;

        DirectX::XMFLOAT3 mPosition;
        DirectX::XMFLOAT3 mScale;

        DirectX::XMMATRIX mProj;
        DirectX::XMFLOAT3 mCameraPosition;
        DirectX::XMFLOAT3 mCameraLookAt;
        DirectX::XMFLOAT3 mCamearUpVec;

        WVPConstantBuffer mWVPcb;
        ID3D11Buffer* mWVPConstantBuffer;
    };
}
