#pragma once

#include <vector>
#include <string>
#include <Windows.h>
#include <DirectXMath.h>

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

private:
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

private:
    bool LoadByJson(const std::string& _path);
    bool LoadByBinary(const std::string& _path);

    std::vector<TempSubMesh> mSubMeshes;
    std::string mDirectory;
    std::string mTextureType;
};
