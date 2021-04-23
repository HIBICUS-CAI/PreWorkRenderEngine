#include "tempMesh.h"

using namespace TEMP;
using namespace DirectX;

SubMesh::SubMesh(ID3D11Device* dev,
    std::vector<MESH_VERTEX> vertices,
    std::vector<UINT> indices,
    std::vector<MESH_TEXTURE> textures)
    :mD3DDev(dev), mVertices(vertices),
    mIndices(indices), mTextures(textures),
    mVertexBuffer(nullptr), mIndexBuffer(nullptr)
{
    SetupSubMesh();
}

SubMesh::~SubMesh()
{

}

void SubMesh::Draw(ID3D11DeviceContext* devContext)
{
    UINT stride = sizeof(MESH_VERTEX);
    UINT offset = 0;

    devContext->IASetVertexBuffers(0, 1, &mVertexBuffer,
        &stride, &offset);
    devContext->IASetIndexBuffer(mIndexBuffer,
        DXGI_FORMAT_R32_UINT, 0);
    //devContext->PSSetShaderResources(0, 1,
        //&mTextures[0].TexResView);

    devContext->DrawIndexed(mIndices.size(), 0, 0);
}

void SubMesh::DeleteThisSubMesh()
{
    if (mVertexBuffer)
    {
        mVertexBuffer->Release();
    }
    if (mIndexBuffer)
    {
        mIndexBuffer->Release();
    }
}

bool SubMesh::SetupSubMesh()
{
    if (!mD3DDev)
    {
        return false;
    }
    HRESULT hr;

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(MESH_VERTEX) * mVertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = &mVertices[0];

    hr = mD3DDev->CreateBuffer(&vbd, &initData, &mVertexBuffer);
    if (FAILED(hr))
    {
        return false;
    }

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * mIndices.size();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;

    initData.pSysMem = &mIndices[0];

    hr = mD3DDev->CreateBuffer(&ibd, &initData, &mIndexBuffer);
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

Mesh::Mesh(ID3D11Device* d3DDevice)
    :mD3DDev(d3DDevice)
{

}

Mesh::~Mesh()
{

}

bool Mesh::Load(std::string fileName)
{
    Assimp::Importer importer;

    const aiScene* pScene = importer.ReadFile(fileName,
        aiProcess_Triangulate |
        aiProcess_ConvertToLeftHanded);

    if (pScene == NULL)
    {
        return false;
    }

    this->mDirectory = fileName.substr(
        0, fileName.find_last_of('/'));

    ProcessNode(pScene->mRootNode, pScene);

    return true;
}

void Mesh::DeleteThisMesh()
{
    for (int i = 0; i < mSubMeshes.size(); i++)
    {
        mSubMeshes[i].DeleteThisSubMesh();
    }

    //mD3DDev->Release();
}

void Mesh::Draw(ID3D11DeviceContext* devContext)
{
    for (int i = 0; i < mSubMeshes.size(); i++)
    {
        mSubMeshes[i].Draw(devContext);
    }
}

void Mesh::ProcessNode(
    aiNode* node, const aiScene* scene)
{
    for (UINT i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        mSubMeshes.push_back(this->ProcessSubMesh(mesh, scene));
    }

    for (UINT i = 0; i < node->mNumChildren; i++)
    {
        this->ProcessNode(node->mChildren[i], scene);
    }
}

SubMesh Mesh::ProcessSubMesh(
    aiMesh* mesh, const aiScene* scene)
{
    // Data to fill
    std::vector<MESH_VERTEX> vertices;
    std::vector<UINT> indices;
    std::vector<MESH_TEXTURE> textures;

    // Walk through each of the mesh's vertices
    for (UINT i = 0; i < mesh->mNumVertices; i++)
    {
        MESH_VERTEX vertex;

        vertex.Position.x = mesh->mVertices[i].x;
        vertex.Position.y = mesh->mVertices[i].y;
        vertex.Position.z = mesh->mVertices[i].z;

        if (mesh->mNormals)
        {
            vertex.Normal.x = mesh->mNormals[i].x;
            vertex.Normal.y = mesh->mNormals[i].y;
            vertex.Normal.z = mesh->mNormals[i].z;
        }

        if (mesh->mTextureCoords[0])
        {
            vertex.TexCoord.x =
                (float)mesh->mTextureCoords[0][i].x;
            vertex.TexCoord.y =
                (float)mesh->mTextureCoords[0][i].y;
        }

        vertices.push_back(vertex);
    }

    for (UINT i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];

        for (UINT j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    /*if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    }*/

    return SubMesh(mD3DDev, vertices, indices, textures);
}
