#include <DirectXTK\WICTextureLoader.h>
#include "tempMyMesh.h"
#include "tempD3d.h"
#include "ShadowTex.h"
#include "SsaoTexs.h"

ID3D11ShaderResourceView* TEMP::CreateTextureView(
    const wchar_t* fileName)
{
    HRESULT hr = S_OK;
    ID3D11ShaderResourceView* texSRV = nullptr;

    hr = DirectX::CreateWICTextureFromFile(
        GetD3DDevicePointer(), fileName, nullptr, &texSRV);
    if (FAILED(hr))
    {
        return nullptr;
    }

    return texSRV;
}

bool TEMP::PrepareTempMyMesh1(
    std::vector<MESH_VERTEX>* vertices,
    std::vector<UINT>* indices,
    std::vector<MESH_TEXTURE>* textures)
{
    MESH_VERTEX v = {};
    v.Normal = { 0.f, 0.f, -1.f };
    v.Position = { -1.f, 0.f, 0.f };
    v.TexCoord = { 0.f, 1.f };
    vertices->push_back(v);
    v.Position = { -1.f, 2.f, 0.f };
    v.TexCoord = { 0.f, 0.f };
    vertices->push_back(v);
    v.Position = { 1.f, 2.f, 0.f };
    v.TexCoord = { 1.f, 0.f };
    vertices->push_back(v);
    v.Position = { 1.f, 0.f, 0.f };
    v.TexCoord = { 1.f, 1.f };
    vertices->push_back(v);

    UINT i;
    i = 0;
    indices->push_back(i);
    i = 1;
    indices->push_back(i);
    i = 2;
    indices->push_back(i);
    i = 2;
    indices->push_back(i);
    i = 3;
    indices->push_back(i);
    i = 0;
    indices->push_back(i);

    MESH_TEXTURE t = {};
    t.TexResView = TEMP::CreateTextureView(
        L"Textures\\white.jpg");
    textures->push_back(t);

    return true;
}

bool TEMP::PrepareTempMyMesh2(
    std::vector<MESH_VERTEX>* vertices,
    std::vector<UINT>* indices,
    std::vector<MESH_TEXTURE>* textures)
{
    MESH_VERTEX v = {};
    v.Normal = { 0.f, 1.f, 0.f };
    v.Position = { -1.f, 0.f, -2.f };
    v.TexCoord = { 0.f, 1.f };
    vertices->push_back(v);
    v.Position = { -1.f, 0.f, 0.f };
    v.TexCoord = { 0.f, 0.f };
    vertices->push_back(v);
    v.Position = { 1.f, 0.f, 0.f };
    v.TexCoord = { 1.f, 0.f };
    vertices->push_back(v);
    v.Position = { 1.f, 0.f, -2.f };
    v.TexCoord = { 1.f, 1.f };
    vertices->push_back(v);

    return true;
}

using namespace TEMP;

MySubMesh::MySubMesh(ID3D11Device* dev, MyMesh* myMesh,
    std::vector<MESH_VERTEX> vertices,
    std::vector<UINT> indices,
    std::vector<MESH_TEXTURE> textures)
    :mVertices(vertices), mIndices(indices), mTextures(textures),
    mD3DDev(dev), mVertexBuffer(nullptr), mIndexBuffer(nullptr),
    mMyMesh(myMesh)
{
    if (!mD3DDev)
    {
        return;
    }

    HRESULT hr = S_OK;

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
        return;
    }

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = (UINT)(sizeof(UINT) * mIndices.size());
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;

    initData.pSysMem = &mIndices[0];

    hr = mD3DDev->CreateBuffer(&ibd, &initData, &mIndexBuffer);
    if (FAILED(hr))
    {
        return;
    }
}

MySubMesh::~MySubMesh()
{

}

void MySubMesh::DeleteThisSubMesh()
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

void MySubMesh::Draw(ID3D11DeviceContext* devContext)
{
    UINT stride = sizeof(MESH_VERTEX);
    UINT offset = 0;

    devContext->IASetVertexBuffers(0, 1, &mVertexBuffer,
        &stride, &offset);
    devContext->IASetIndexBuffer(mIndexBuffer,
        DXGI_FORMAT_R32_UINT, 0);
    ID3D11Buffer* buffer = mMyMesh->GetWVPBufferPtr();
    devContext->VSSetConstantBuffers(
        0, 1, &buffer);
    devContext->PSSetShaderResources(0, 1,
        &mTextures[0].TexResView);

    devContext->DrawIndexed((UINT)mIndices.size(), 0, 0);
}

MyMesh::MyMesh(ID3D11Device* d3DDevice)
    :mD3DDev(d3DDevice)
{
    mProj = GetProjMat();
    mCameraPosition = GetEyePos();
    mCameraLookAt = GetEyeLookat();
    mCamearUpVec = GetEyeUp();

    mWVPConstantBuffer = nullptr;
    D3D11_BUFFER_DESC bdc = {};
    bdc.Usage = D3D11_USAGE_DEFAULT;
    bdc.ByteWidth = sizeof(WVPConstantBuffer);
    bdc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bdc.CPUAccessFlags = 0;
    mD3DDev->CreateBuffer(
        &bdc, nullptr, &mWVPConstantBuffer);

    mPosition = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
    mScale = DirectX::XMFLOAT3(1.f, 1.f, 1.f);

    mWVPcb = {};
}

MyMesh::~MyMesh()
{

}

void MyMesh::CreateSub(
    std::vector<MESH_VERTEX> vertices,
    std::vector<UINT> indices,
    std::vector<MESH_TEXTURE> textures
)
{
    mSubMeshes.push_back(MySubMesh(mD3DDev, this, vertices,
        indices, textures));
}

void MyMesh::DeleteThisMesh()
{
    for (int i = 0; i < mSubMeshes.size(); i++)
    {
        mSubMeshes[i].DeleteThisSubMesh();
    }
}

void MyMesh::DrawShadowDepth(ID3D11DeviceContext* devContext)
{
    DirectX::XMMATRIX world =
        DirectX::XMMatrixMultiply(
            DirectX::XMMatrixScalingFromVector(
                DirectX::XMLoadFloat3(&mScale)),
            DirectX::XMMatrixTranslationFromVector(
                DirectX::XMLoadFloat3(&mPosition)));

    /*DirectX::XMFLOAT4 v4 = {
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
    DirectX::XMMATRIX view =
        DirectX::XMMatrixLookAtLH(eye, lookat, up);

    DirectX::XMMATRIX proj = GetProjMat();*/

    mWVPcb.mWorld = DirectX::XMMatrixTranspose(world);
    mWVPcb.mView = DirectX::XMMatrixTranspose(GetLughtVM());
    mWVPcb.mProjection = DirectX::XMMatrixTranspose(GetLughtOM());

    devContext->UpdateSubresource(
        mWVPConstantBuffer, 0, nullptr, &mWVPcb, 0, 0);

    for (int i = 0; i < mSubMeshes.size(); i++)
    {
        mSubMeshes[i].Draw(devContext);
    }
}

void MyMesh::Draw(ID3D11DeviceContext* devContext)
{
    DirectX::XMMATRIX world =
        DirectX::XMMatrixMultiply(
            DirectX::XMMatrixScalingFromVector(
                DirectX::XMLoadFloat3(&mScale)),
            DirectX::XMMatrixTranslationFromVector(
                DirectX::XMLoadFloat3(&mPosition)));

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
    DirectX::XMMATRIX view =
        DirectX::XMMatrixLookAtLH(eye, lookat, up);

    DirectX::XMMATRIX proj = GetProjMat();

    mWVPcb.mWorld = DirectX::XMMatrixTranspose(world);
    mWVPcb.mView = DirectX::XMMatrixTranspose(view);
    mWVPcb.mProjection = DirectX::XMMatrixTranspose(proj);
    mWVPcb.mShadowView = DirectX::XMMatrixTranspose(GetLughtVM());
    mWVPcb.mShadowProjection = DirectX::XMMatrixTranspose(
        GetLughtOM());
    DirectX::XMMATRIX T(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f);
    mWVPcb.mSsaoVPT = DirectX::XMMatrixTranspose(
        view * proj * T);

    devContext->UpdateSubresource(
        mWVPConstantBuffer, 0, nullptr, &mWVPcb, 0, 0);

    static ID3D11ShaderResourceView* shadow = nullptr;
    shadow = GetShadow()->GetSRV();
    devContext->PSSetShaderResources(1, 1, &shadow);

    static ID3D11ShaderResourceView* ssao = nullptr;
    ssao = GetSsao()->GetSsaoMap();
    devContext->PSSetShaderResources(2, 1, &ssao);

    for (int i = 0; i < mSubMeshes.size(); i++)
    {
        mSubMeshes[i].Draw(devContext);
    }
}

void MyMesh::DrawSsaoNormal(ID3D11DeviceContext* devContext)
{
    DirectX::XMMATRIX world =
        DirectX::XMMatrixMultiply(
            DirectX::XMMatrixScalingFromVector(
                DirectX::XMLoadFloat3(&mScale)),
            DirectX::XMMatrixTranslationFromVector(
                DirectX::XMLoadFloat3(&mPosition)));

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
    DirectX::XMMATRIX view =
        DirectX::XMMatrixLookAtLH(eye, lookat, up);

    DirectX::XMMATRIX proj = GetProjMat();

    mWVPcb.mWorld = DirectX::XMMatrixTranspose(world);
    mWVPcb.mView = DirectX::XMMatrixTranspose(view);
    mWVPcb.mProjection = DirectX::XMMatrixTranspose(proj);
    mWVPcb.mShadowView = DirectX::XMMatrixTranspose(GetLughtVM());
    mWVPcb.mShadowProjection = DirectX::XMMatrixTranspose(
        GetLughtOM());

    devContext->UpdateSubresource(
        mWVPConstantBuffer, 0, nullptr, &mWVPcb, 0, 0);

    devContext->PSSetConstantBuffers(
        0, 1, &mWVPConstantBuffer);

    for (int i = 0; i < mSubMeshes.size(); i++)
    {
        mSubMeshes[i].Draw(devContext);
    }
}
