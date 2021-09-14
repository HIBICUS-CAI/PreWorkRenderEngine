#define _CRT_SECURE_NO_WARNINGS

#include "TempMesh.h"
#include <fstream>
#include <cstdio>
#include <rapidjson\filereadstream.h>
#include <rapidjson\writer.h>
#include <rapidjson\document.h>

TempSubMesh::TempSubMesh(
    std::vector<UINT>& _index,
    std::vector<VERTEX_INFO>& _vertex,
    std::vector<TEXTURE_INFO>& _tex) :
    mIndeices(_index), mVerteices(_vertex), mTextures(_tex) {}

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
    return true;
}
