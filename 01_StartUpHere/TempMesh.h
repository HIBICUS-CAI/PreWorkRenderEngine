#pragma once

#include <vector>
#include <string>
#include <Windows.h>
#include <DirectXMath.h>
#include "RSCommon.h"
#include "RSMeshHelper.h"
#include "RSDrawCallsPool.h"
#include "RSRoot_DX11.h"
#include "RSTexturesManager.h"
#include "RSCamerasContainer.h"

enum class MESH_FILE_TYPE
{
    BIN,
    JSON
};

struct VERTEX_INFO
{
    DirectX::XMFLOAT3 mPosition = { 0.f,0.f,0.f };
    DirectX::XMFLOAT3 mNormal = { 0.f,0.f,0.f };
    DirectX::XMFLOAT3 mTangent = { 0.f,0.f,0.f };
    DirectX::XMFLOAT2 mTexCoord = { 0.f,0.f };
};

struct TEXTURE_INFO
{
    std::string mType = "";
    std::string mPath = "";
};

class TempSubMesh
{
public:
    TempSubMesh(
        std::vector<UINT>& _index,
        std::vector<VERTEX_INFO>& _vertex,
        std::vector<TEXTURE_INFO>& _tex);
    ~TempSubMesh();

    void Process(RSMeshHelper* _helper)
    {
        SUBMESH_INFO si = {};
        si.mTopologyType = TOPOLOGY_TYPE::TRIANGLELIST;
        si.mIndeices = &mIndeices;
        si.mVerteices = &mVerteices;
        std::vector<std::string> t = {};
        for (auto& tex : mTextures)
        {
            t.emplace_back(tex.mPath);
        }
        si.mTextures = &t;
        si.mStaticMaterial = "";
        MATERIAL_INFO mi = {};
        mi.mDiffuseAlbedo = {};
        mi.mFresnelR0 = {};
        mi.mShininess = 5.f;
        si.mMaterial = &mi;
        _helper->ProcessSubMesh(&mData,
            &si, LAYOUT_TYPE::NORMAL_TANGENT_TEX);
        //mData=_helper->ProcessSubMesh()
    }

    void UploadDrawCall(RSDrawCallsPool* _pool,
        RSRoot_DX11* _root)
    {
        static std::vector<RS_INSTANCE_DATA> instance = {};
        static float time = 0.f;
        time += 0.0001f;
        instance.clear();
        {
            RS_INSTANCE_DATA ins_data = {};
            DirectX::XMMATRIX mat = {};
            DirectX::XMFLOAT4X4 flt44 = {};
            mat = DirectX::XMMatrixMultiply(
                DirectX::XMMatrixScaling(0.04f, 0.04f, 0.04f),
                DirectX::XMMatrixRotationY(time)
            );
            mat = DirectX::XMMatrixMultiply(
                mat,
                DirectX::XMMatrixTranslation(0.f, 0.f, 10.f));
            DirectX::XMStoreFloat4x4(&flt44, mat);
            ins_data.mWorldMat = flt44;
            instance.emplace_back(ins_data);
        }

        std::string name = "temp-cam";
        RS_DRAWCALL_DATA data = {};
        data.mMeshData.mLayout = mData.mLayout;
        data.mMeshData.mTopologyType = mData.mTopologyType;
        data.mMeshData.mIndexBuffer = mData.mIndexBuffer;
        data.mMeshData.mVertexBuffer = mData.mVertexBuffer;
        data.mMeshData.mIndexCount = (UINT)mIndeices.size();
        data.mInstanceData.mDataPtr = &instance;
        data.mCameraData = *(_root->CamerasContainer()->
            GetRSCameraInfo(name));
        data.mTextureDatas[0].mUse = true;
        data.mTextureDatas[0].mSrv = _root->TexturesManager()->
            GetMeshSrv(mData.mTextures[0]);

        _pool->AddDrawCallToPipe(DRAWCALL_TYPE::OPACITY, data);
    }

    void Release(RSMeshHelper* _helper)
    {
        _helper->ReleaseSubMesh(mData);
    }

private:
    RS_SUBMESH_DATA mData;

    std::vector<UINT> mIndeices;
    std::vector<VERTEX_INFO> mVerteices;
    std::vector<TEXTURE_INFO> mTextures;
};

class TempMesh
{
public:
    TempMesh();
    ~TempMesh();

    bool Load(const std::string& _filePath, MESH_FILE_TYPE _type);

    void Process(RSMeshHelper* _helper)
    {
        for (auto& sub : mSubMeshes)
        {
            sub.Process(_helper);
        }
    }

    void UploadDrawCall(RSDrawCallsPool* _pool,
        RSRoot_DX11* _root)
    {
        for (auto& sub : mSubMeshes)
        {
            sub.UploadDrawCall(_pool, _root);
        }
    }

    void Release(RSMeshHelper* _helper)
    {
        for (auto& sub : mSubMeshes)
        {
            sub.Release(_helper);
        }
    }

private:
    bool LoadByJson(const std::string& _path);
    bool LoadByBinary(const std::string& _path);

    std::vector<TempSubMesh> mSubMeshes;
    std::string mDirectory;
    std::string mTextureType;
};
