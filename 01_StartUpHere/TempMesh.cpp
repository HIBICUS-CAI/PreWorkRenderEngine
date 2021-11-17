#define _CRT_SECURE_NO_WARNINGS

#include "TempMesh.h"
#include <fstream>
#include <cstdio>
#include <cstring>
#include <rapidjson\filereadstream.h>
#include <rapidjson\writer.h>
#include <rapidjson\document.h>
#include <DirectXTK\DDSTextureLoader.h>
#include <DirectXTK\WICTextureLoader.h>
#include "RSDevices.h"
#include "RSResourceManager.h"

TempSubMesh::TempSubMesh(
    std::vector<UINT>& _index,
    std::vector<VERTEX_INFO>& _vertex,
    std::vector<TEXTURE_INFO>& _tex) :
    mIndeices(_index), mVerteices(_vertex), mTextures(_tex),
    mData({})
{

}

TempSubMesh::~TempSubMesh() {}

TempMesh::TempMesh() :
    mSubMeshes({}), mDirectory(""), mTextureType("") {}

TempMesh::~TempMesh() {}

bool TempMesh::Load(const std::string& _path, MESH_FILE_TYPE _type)
{
    switch (_type)
    {
    case MESH_FILE_TYPE::BIN:
        return LoadByBinary(_path);
    case MESH_FILE_TYPE::JSON:
        return LoadByJson(_path);
    default:
        return false;
    }
}

bool TempMesh::LoadByJson(const std::string& _path)
{
    std::FILE* fp = std::fopen(_path.c_str(), "rb");
    if (!fp) { return false; }
    char* readBuf = new char[65536];
    if (!readBuf) { return false; }

    rapidjson::FileReadStream is(
        fp, readBuf, 65536 * sizeof(char));
    rapidjson::Document doc = {};
    doc.ParseStream(is);
    if (doc.HasParseError())
    {
        char str[64] = "";
        sprintf_s(str, 64, "error code : %d\n",
            doc.GetParseError());
        OutputDebugString(str);
        return false;
    }
    delete[] readBuf;
    std::fclose(fp);

    mDirectory = doc["directory"].GetString();
    mTextureType = doc["texture-type"].GetString();

    UINT subSize = doc["sub-model-size"].GetUint();
    mSubMeshes.reserve((size_t)subSize);
    UINT subArraySize = doc["sub-model"].Size();
    if (subSize != subArraySize) { return false; }

    std::vector<UINT> index = {};
    std::vector<VERTEX_INFO> vertex = {};
    std::vector<TEXTURE_INFO> texture = {};
    for (UINT i = 0; i < subSize; i++)
    {
        index.clear();
        vertex.clear();
        texture.clear();
        UINT indexSize =
            doc["sub-model"][i]["index-size"].GetUint();
        if (indexSize !=
            (UINT)doc["sub-model"][i]["index"].Size())
        {
            return false;
        }
        UINT vertexSize =
            doc["sub-model"][i]["vertex-size"].GetUint();
        if (vertexSize !=
            (UINT)doc["sub-model"][i]["vertex"].Size())
        {
            return false;
        }
        UINT texSize =
            doc["sub-model"][i]["texture-size"].GetUint();
        if (texSize !=
            (UINT)doc["sub-model"][i]["texture"].Size())
        {
            return false;
        }

        for (UINT j = 0; j < indexSize; j++)
        {
            index.push_back(
                doc["sub-model"][i]["index"][j].GetUint());
        }

        for (UINT j = 0; j < vertexSize; j++)
        {
            static VERTEX_INFO v = {};
            v = {};

            v.mPosition.x =
                doc["sub-model"][i]["vertex"][j]["position"][0].
                GetFloat();
            v.mPosition.y =
                doc["sub-model"][i]["vertex"][j]["position"][1].
                GetFloat();
            v.mPosition.z =
                doc["sub-model"][i]["vertex"][j]["position"][2].
                GetFloat();

            v.mNormal.x =
                doc["sub-model"][i]["vertex"][j]["normal"][0].
                GetFloat();
            v.mNormal.y =
                doc["sub-model"][i]["vertex"][j]["normal"][1].
                GetFloat();
            v.mNormal.z =
                doc["sub-model"][i]["vertex"][j]["normal"][2].
                GetFloat();

            v.mTangent.x =
                doc["sub-model"][i]["vertex"][j]["tangent"][0].
                GetFloat();
            v.mTangent.y =
                doc["sub-model"][i]["vertex"][j]["tangent"][1].
                GetFloat();
            v.mTangent.z =
                doc["sub-model"][i]["vertex"][j]["tangent"][2].
                GetFloat();

            v.mTexCoord.x =
                doc["sub-model"][i]["vertex"][j]["texcoord"][0].
                GetFloat();
            v.mTexCoord.y =
                doc["sub-model"][i]["vertex"][j]["texcoord"][1].
                GetFloat();

            vertex.push_back(v);
        }

        for (UINT j = 0; j < texSize; j++)
        {
            static TEXTURE_INFO t = {};
            t = {};
            t.mType =
                doc["sub-model"][i]["texture"][j]["type"].
                GetString();
            t.mPath =
                doc["sub-model"][i]["texture"][j]["path"].
                GetString();

            texture.push_back(t);
        }

        mSubMeshes.push_back(TempSubMesh(index, vertex, texture));
    }

    return true;
}

bool TempMesh::LoadByBinary(const std::string& _path)
{
    std::ifstream inFile(_path, std::ios::in | std::ios::binary);

    if (!inFile.is_open()) { return false; }

    // directory
    {
        int size = 0;
        char str[128] = "";
        inFile.read((char*)&size, sizeof(size));
        inFile.read(str, size);
        mDirectory = str;
    }

    // texture-type
    {
        int size = 0;
        char str[128] = "";
        inFile.read((char*)&size, sizeof(size));
        inFile.read(str, size);
        mTextureType = str;
    }

    // sub-model-size
    int subSize = 0;
    inFile.read((char*)&subSize, sizeof(subSize));
    mSubMeshes.reserve((size_t)subSize);

    std::vector<UINT> index = {};
    std::vector<VERTEX_INFO> vertex = {};
    std::vector<TEXTURE_INFO> texture = {};
    int indexSize = 0;
    int vertexSize = 0;
    int textureSize = 0;
    for (int i = 0; i < subSize; i++)
    {
        index.clear();
        vertex.clear();
        texture.clear();

        // each-sub-sizes
        inFile.read((char*)&indexSize, sizeof(indexSize));
        inFile.read((char*)&vertexSize, sizeof(vertexSize));
        inFile.read((char*)&textureSize, sizeof(textureSize));

        // each-sub-index
        UINT ind = 0;
        VERTEX_INFO ver = {};
        TEXTURE_INFO tex = {};
        for (int j = 0; j < indexSize; j++)
        {
            inFile.read((char*)&ind, sizeof(ind));
            index.push_back(ind);
        }

        // each-sub-vertex
        {
            double temp = 0.0;
            for (int j = 0; j < vertexSize; j++)
            {
                inFile.read((char*)(&temp),
                    sizeof(double));
                ver.mPosition.x = (float)temp;
                inFile.read((char*)(&temp),
                    sizeof(double));
                ver.mPosition.y = (float)temp;
                inFile.read((char*)(&temp),
                    sizeof(double));
                ver.mPosition.z = (float)temp;

                inFile.read((char*)(&temp),
                    sizeof(double));
                ver.mNormal.x = (float)temp;
                inFile.read((char*)(&temp),
                    sizeof(double));
                ver.mNormal.y = (float)temp;
                inFile.read((char*)(&temp),
                    sizeof(double));
                ver.mNormal.z = (float)temp;

                inFile.read((char*)(&temp),
                    sizeof(double));
                ver.mTangent.x = (float)temp;
                inFile.read((char*)(&temp),
                    sizeof(double));
                ver.mTangent.y = (float)temp;
                inFile.read((char*)(&temp),
                    sizeof(double));
                ver.mTangent.z = (float)temp;

                inFile.read((char*)(&temp),
                    sizeof(double));
                ver.mTexCoord.x = (float)temp;
                inFile.read((char*)(&temp),
                    sizeof(double));
                ver.mTexCoord.y = (float)temp;

                vertex.push_back(ver);
            }
        }

        // each-sub-texture
        for (int j = 0; j < textureSize; j++)
        {
            int len = 0;
            char str[1024] = "";
            inFile.read((char*)(&len), sizeof(len));
            inFile.read(str, len);
            tex.mType = str;
            std::strcpy(str, "");
            inFile.read((char*)(&len), sizeof(len));
            inFile.read(str, len);
            tex.mPath = str;
            texture.push_back(tex);
        }

        mSubMeshes.push_back(TempSubMesh(index, vertex, texture));
    }

    inFile.close();

    return true;
}

bool TempGeoMesh::CreateBumpedTex(std::string&& _texPath,
    RSDevices* _devices, RSResourceManager* _texManager)
{
    static std::wstring wstr = L"";
    static std::string name = "";
    static HRESULT hr = S_OK;
    ID3D11ShaderResourceView* srv = nullptr;

    wstr = std::wstring(_texPath.begin(), _texPath.end());
    wstr = L".\\Textures\\" + wstr;

    if (_texPath.find(".dds") != std::string::npos ||
        _texPath.find(".DDS") != std::string::npos)
    {
        hr = DirectX::CreateDDSTextureFromFile(
            _devices->GetDevice(),
            wstr.c_str(), nullptr, &srv);
        if (SUCCEEDED(hr))
        {
            name = _texPath;
            _texManager->AddMeshSrv(name, srv);
        }
        else
        {
            bool texture_load_fail = false;
            assert(texture_load_fail);
            return false;
        }
    }
    else
    {
        hr = DirectX::CreateWICTextureFromFile(
            _devices->GetDevice(),
            wstr.c_str(), nullptr, &srv);
        if (SUCCEEDED(hr))
        {
            name = _texPath;
            _texManager->AddMeshSrv(name, srv);
        }
        else
        {
            bool texture_load_fail = false;
            assert(texture_load_fail);
            return false;
        }
    }

    mData.mTextures.emplace_back(name);
    return true;
}
