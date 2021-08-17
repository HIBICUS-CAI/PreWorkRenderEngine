#include <DirectXTK\WICTextureLoader.h>
#include "tempMesh.h"
#include "tempD3d.h"

namespace TEMP
{
    ID3D11VertexShader* gp_MVertexShader = nullptr;
    ID3D11PixelShader* gp_MPixelShader = nullptr;
    ID3D11InputLayout* gp_MVertexLayout = nullptr;
    ID3D11Buffer* gp_WVPConstantBuffer = nullptr;
    ID3D11SamplerState* gp_TexSamplerState = nullptr;
    DirectX::XMMATRIX g_MWorld;
    DirectX::XMMATRIX g_MView;
    DirectX::XMMATRIX g_MProjection;
    /*DirectX::XMFLOAT3 g_MCameraPosition;
    DirectX::XMFLOAT3 g_MCameraLookAt;
    DirectX::XMFLOAT3 g_MCamearUpVec;*/
    DirectX::XMFLOAT3 g_MLightDirection;

    struct WVPConstantBuffer
    {
        DirectX::XMMATRIX mWorld;
        DirectX::XMMATRIX mView;
        DirectX::XMMATRIX mProjection;
    };

    WVPConstantBuffer g_WVPcb;
}

using namespace TEMP;
using namespace DirectX;

HRESULT TEMP::PrepareMeshD3D(ID3D11Device* dev,
    HWND wndHandle)
{
    HRESULT hr = S_OK;

    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile(
        L"tempMeshVS.hlsl", "main",
        "vs_5_0", &pVSBlob
    );
    if (FAILED(hr))
    {
        return hr;
    }
    hr = dev->CreateVertexShader(pVSBlob->GetBufferPointer(),
        pVSBlob->GetBufferSize(), nullptr, &gp_MVertexShader);
    if (FAILED(hr))
    {
        return hr;
    }
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {
            "POSITION",0,
            DXGI_FORMAT_R32G32B32_FLOAT,0,0,
            D3D11_INPUT_PER_VERTEX_DATA,0
        },
        {
            "NORMAL",0,
            DXGI_FORMAT_R32G32B32_FLOAT,0,12,
            D3D11_INPUT_PER_VERTEX_DATA,0
        },
        {
            "TEXCOORD",0,
            DXGI_FORMAT_R32G32_FLOAT,0,24,
            D3D11_INPUT_PER_VERTEX_DATA,0
        }
    };
    UINT numInputLayouts = ARRAYSIZE(layout);
    hr = dev->CreateInputLayout(
        layout, numInputLayouts,
        pVSBlob->GetBufferPointer(),
        pVSBlob->GetBufferSize(),
        &gp_MVertexLayout);
    if (FAILED(hr))
    {
        return hr;
    }
    pVSBlob->Release();

    ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(
        L"tempMeshPS.hlsl", "main",
        "ps_5_0", &pPSBlob
    );
    if (FAILED(hr))
    {
        return hr;
    }
    hr = dev->CreatePixelShader(pPSBlob->GetBufferPointer(),
        pPSBlob->GetBufferSize(), nullptr, &gp_MPixelShader);
    if (FAILED(hr))
    {
        return hr;
    }
    pPSBlob->Release();

    D3D11_BUFFER_DESC bdc = {};
    bdc.Usage = D3D11_USAGE_DEFAULT;
    bdc.ByteWidth = sizeof(WVPConstantBuffer);
    bdc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bdc.CPUAccessFlags = 0;
    hr = dev->CreateBuffer(
        &bdc, nullptr, &gp_WVPConstantBuffer);
    if (FAILED(hr))
    {
        return hr;
    }

    g_MWorld = DirectX::XMMatrixIdentity();
    /*g_MCameraPosition = { 0.f,0.f,-15.f };
    g_MCameraLookAt = { 0.f,0.f,1.f };
    g_MCamearUpVec = { 0.f,1.f,0.f };
    g_MLightDirection = g_MCameraLookAt;
    DirectX::XMVECTOR eye = DirectX::XMVectorSet(
        g_MCameraPosition.x,
        g_MCameraPosition.y,
        g_MCameraPosition.z,
        0.f);
    DirectX::XMVECTOR lookat = DirectX::XMVectorSet(
        g_MCameraLookAt.x,
        g_MCameraLookAt.y,
        g_MCameraLookAt.z,
        0.f);
    DirectX::XMVECTOR up = DirectX::XMVectorSet(
        g_MCamearUpVec.x,
        g_MCamearUpVec.y,
        g_MCamearUpVec.z,
        0.f);
    g_MView = DirectX::XMMatrixLookAtLH(eye, lookat, up);*/
    RECT rc;
    GetClientRect(wndHandle, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;
    g_MProjection = DirectX::XMMatrixPerspectiveFovLH(
        DirectX::XM_PI / 6.f,
        width / (FLOAT)height, 0.01f, 100.f);

    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    hr = dev->CreateSamplerState(&sampDesc, &gp_TexSamplerState);
    if (FAILED(hr))
    {
        return hr;
    }

    return S_OK;
}

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
    devContext->VSSetConstantBuffers(
        0, 1, &gp_WVPConstantBuffer);
    devContext->PSSetShaderResources(0, 1,
        &mTextures[0].TexResView);

    devContext->DrawIndexed((UINT)mIndices.size(), 0, 0);
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
    vbd.ByteWidth =
        (UINT)(sizeof(MESH_VERTEX) * mVertices.size());
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
    ibd.ByteWidth =
        (UINT)(sizeof(UINT) * mIndices.size());
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
    static float time = 0.0f;
    if (TEMP::GetDriverType() == D3D_DRIVER_TYPE_REFERENCE)
    {
        time += (float)DirectX::XM_PI * 0.0125f;
    }
    else
    {
        static ULONGLONG timeStart = 0;
        ULONGLONG timeCur = GetTickCount64();
        if (timeStart == 0)
            timeStart = timeCur;
        time = (timeCur - timeStart) / 1000.0f;
    }

    g_MWorld = DirectX::XMMatrixMultiply(
        DirectX::XMMatrixScaling(0.03f, 0.03f, 0.03f),
        DirectX::XMMatrixRotationY(time)
    );
    g_MWorld = DirectX::XMMatrixMultiply(
        g_MWorld,
        DirectX::XMMatrixTranslation(0.f, -2.f, 0.f)
    );
    DirectX::XMFLOAT4 v4 = {
            GetEyePos().x,
            GetEyePos().y,
            GetEyePos().z,
            0.f };
    DirectX::XMVECTOR eye = DirectX::XMLoadFloat4(&v4);
    DirectX::XMVECTOR lookat = DirectX::XMVectorSet(
        GetEyePos().x + GetEyeLookat().x,
        GetEyePos().y + GetEyeLookat().y,
        GetEyePos().z + GetEyeLookat().z, 0.f);
    DirectX::XMVECTOR up = DirectX::XMVectorSet(
        GetEyeUp().x, GetEyeUp().y, GetEyeUp().z, 0.f);
    g_MView = DirectX::XMMatrixLookAtLH(eye, lookat, up);

    g_WVPcb.mWorld = DirectX::XMMatrixTranspose(g_MWorld);
    g_WVPcb.mView = DirectX::XMMatrixTranspose(g_MView);
    g_WVPcb.mProjection = DirectX::XMMatrixTranspose(
        g_MProjection);

    devContext->UpdateSubresource(
        gp_WVPConstantBuffer, 0, nullptr, &g_WVPcb, 0, 0);
    devContext->IASetInputLayout(TEMP::gp_MVertexLayout);
    devContext->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    devContext->PSSetSamplers(0, 1, &gp_TexSamplerState);
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

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* mat =
            scene->mMaterials[mesh->mMaterialIndex];
        if (mTextureType.empty())
        {
            mTextureType =
                DetermineDiffuseTextureType(scene, mat);
        }
    }

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

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<MESH_TEXTURE> diffuseMaps =
            LoadMaterialTextures(
                material, aiTextureType_DIFFUSE,
                "texture_diffuse", scene);
        textures.insert(textures.end(),
            diffuseMaps.begin(), diffuseMaps.end());
    }

    return SubMesh(mD3DDev, vertices, indices, textures);
}

std::vector<MESH_TEXTURE> Mesh::LoadMaterialTextures(
    aiMaterial* mat, aiTextureType type,
    std::string typeName, const aiScene* scene)
{
    std::vector<MESH_TEXTURE> textures;
    for (UINT i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (UINT j = 0; j < mLoadedTexs.size(); j++)
        {
            if (std::strcmp(mLoadedTexs[j].Path.c_str(),
                str.C_Str()) == 0)
            {
                textures.push_back(mLoadedTexs[j]);
                skip = true;
                break;
            }
        }
        if (!skip)
        {
            HRESULT hr = S_OK;
            MESH_TEXTURE meshTex;
            if (mTextureType == "embedded compressed texture")
            {
                int textureIndex = GetTextureIndex(&str);
                meshTex.TexResView = GetTextureFromModel(
                    scene, textureIndex);
            }
            else
            {
                std::string fileName = std::string(str.C_Str());
                fileName = fileName.substr(
                    fileName.find_last_of("\\") + 1);
                fileName = "Textures\\" + fileName;
                std::wstring wFileName = std::wstring(
                    fileName.begin(), fileName.end());
                hr = CreateWICTextureFromFile(mD3DDev,
                    wFileName.c_str(), nullptr,
                    &meshTex.TexResView);
                if (FAILED(hr))
                {
                    return std::vector<MESH_TEXTURE>(0);
                }
            }
            meshTex.Type = typeName;
            meshTex.Path = str.C_Str();
            textures.push_back(meshTex);
            mLoadedTexs.push_back(meshTex);
        }
    }
    return textures;
}

std::string Mesh::DetermineDiffuseTextureType(
    const aiScene* scene, aiMaterial* mat)
{
    aiString aiTexTypeStr;
    mat->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexTypeStr);
    std::string texTypeStr = aiTexTypeStr.C_Str();
    if (texTypeStr == "*0" || texTypeStr == "*1" ||
        texTypeStr == "*2" || texTypeStr == "*3" ||
        texTypeStr == "*4" || texTypeStr == "*5")
    {
        if (scene->mTextures[0]->mHeight == 0)
        {
            return "embedded compressed texture";
        }
        else
        {
            return "embedded non-compressed texture";
        }
    }
    if (texTypeStr.find('.') != std::string::npos)
    {
        return "textures are on disk";
    }

    return "no path to go";
}

int Mesh::GetTextureIndex(aiString* str)
{
    std::string tistr;
    tistr = str->C_Str();
    tistr = tistr.substr(1);
    return stoi(tistr);
}

ID3D11ShaderResourceView* Mesh::GetTextureFromModel(
    const aiScene* scene, int texIndex)
{
    HRESULT hr = S_OK;
    ID3D11ShaderResourceView* texSRV = nullptr;

    int* size = reinterpret_cast<int*>(
        &scene->mTextures[texIndex]->mWidth);

    hr = CreateWICTextureFromMemory(
        mD3DDev, GetD3DDevContPointer(),
        reinterpret_cast<unsigned char*>(
            scene->mTextures[texIndex]->pcData),
        *size, nullptr, &texSRV);
    if (FAILED(hr))
    {
        return nullptr;
    }

    return texSRV;
}
