#pragma once

#include <vector>
#include <string>
#include <Windows.h>
#include <DirectXMath.h>
#include "RSCommon.h"
#include "RSMeshHelper.h"

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
        std::vector<void*> p = {};
        for (auto& vert : mVerteices)
        {
            p.push_back(&vert);
        }
        si.mVerteices = &p;
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
        mData = _helper->ProcessSubMesh(
            &si, LAYOUT_TYPE::NORMAL_TANGENT_TEX);
        //mData=_helper->ProcessSubMesh()
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

private:
    bool LoadByJson(const std::string& _path);
    bool LoadByBinary(const std::string& _path);

    std::vector<TempSubMesh> mSubMeshes;
    std::string mDirectory;
    std::string mTextureType;
};
