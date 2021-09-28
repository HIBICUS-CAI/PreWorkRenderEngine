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
#include "RSStaticResources.h"

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
                DirectX::XMMatrixScaling(0.15f, 0.15f, 0.15f),
                DirectX::XMMatrixRotationY(time)
            );
            mat = DirectX::XMMatrixMultiply(
                mat,
                DirectX::XMMatrixTranslation(0.f, 2.6f, 0.f));
            DirectX::XMStoreFloat4x4(&flt44, mat);
            ins_data.mWorldMat = flt44;
            instance.emplace_back(ins_data);

            ins_data = {};
            mat = DirectX::XMMatrixMultiply(
                DirectX::XMMatrixScaling(0.02f, 0.02f, 0.02f),
                DirectX::XMMatrixRotationY(0.f)
            );
            mat = DirectX::XMMatrixMultiply(
                mat,
                DirectX::XMMatrixTranslation(-8.5f, -4.f, 0.f));
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
        data.mMeshData.mIndexCount = mData.mIndexCount;
        data.mInstanceData.mDataPtr = &instance;
        data.mCameraData = *(_root->CamerasContainer()->
            GetRSCameraInfo(name));
        data.mTextureDatas[0].mUse = true;
        data.mTextureDatas[0].mSrv = _root->TexturesManager()->
            GetMeshSrv(mData.mTextures[0]);
        name = "copper";
        data.mMaterialData = *(_root->StaticResources()->
            GetStaticMaterial(name));

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

class TempGeoMesh
{
public:
    TempGeoMesh(RS_SUBMESH_DATA&& _data) :mData(_data) {}
    ~TempGeoMesh() {}

    void UploadDrawCall(RSDrawCallsPool* _pool,
        RSRoot_DX11* _root)
    {
        mInstance.clear();

        for (size_t i = 0; i < mPostions.size(); i++)
        {
            RS_INSTANCE_DATA ins_data = {};
            DirectX::XMMATRIX mat = {};
            DirectX::XMFLOAT4X4 flt44 = {};
            mat = DirectX::XMMatrixMultiply(
                DirectX::XMMatrixScaling(
                    mScales[i].x,
                    mScales[i].y,
                    mScales[i].z),
                DirectX::XMMatrixRotationRollPitchYaw(
                    mRotations[i].x,
                    mRotations[i].y,
                    mRotations[i].z)
            );
            mat = DirectX::XMMatrixMultiply(
                mat,
                DirectX::XMMatrixTranslation(
                    mPostions[i].x,
                    mPostions[i].y,
                    mPostions[i].z));
            DirectX::XMStoreFloat4x4(&flt44, mat);
            ins_data.mWorldMat = flt44;
            mInstance.emplace_back(ins_data);
        }

        std::string name = "temp-cam";
        RS_DRAWCALL_DATA data = {};
        data.mMeshData.mLayout = mData.mLayout;
        data.mMeshData.mTopologyType = mData.mTopologyType;
        data.mMeshData.mIndexBuffer = mData.mIndexBuffer;
        data.mMeshData.mVertexBuffer = mData.mVertexBuffer;
        data.mMeshData.mIndexCount = mData.mIndexCount;
        data.mInstanceData.mDataPtr = &mInstance;
        data.mCameraData = *(_root->CamerasContainer()->
            GetRSCameraInfo(name));
        data.mTextureDatas[0].mUse = true;
        data.mTextureDatas[0].mSrv = _root->TexturesManager()->
            GetMeshSrv(mData.mTextures[0]);
        name = "copper";
        data.mMaterialData = *(_root->StaticResources()->
            GetStaticMaterial(name));

        _pool->AddDrawCallToPipe(DRAWCALL_TYPE::OPACITY, data);
    }

    void Release(RSMeshHelper* _helper)
    {
        if (_helper)
        {
            _helper->ReleaseSubMesh(mData);
        }
    }

    void AddInstanceData(DirectX::XMFLOAT3 _pos,
        DirectX::XMFLOAT3 _angle, DirectX::XMFLOAT3 _size)
    {
        mPostions.emplace_back(_pos);
        mRotations.emplace_back(_angle);
        mScales.emplace_back(_size);
    }

private:
    RS_SUBMESH_DATA mData;

    std::vector<DirectX::XMFLOAT3> mPostions = {};
    std::vector<DirectX::XMFLOAT3> mRotations = {};
    std::vector<DirectX::XMFLOAT3> mScales = {};

    std::vector<RS_INSTANCE_DATA> mInstance = {};
};
