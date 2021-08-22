#include "SsaoTexs.h"
#include <Windows.h>
#include <DirectXMath.h>
#include "tempDeclaration.h"
#include "tempD3d.h"

SsaoTexs* g_Ssao = nullptr;

struct SSAOConstantBuffer
{
    DirectX::XMMATRIX mProj;
    DirectX::XMMATRIX mInvProj;
};

SSAOConstantBuffer g_SSAOcb;

SsaoTexs* GetSsao()
{
    return g_Ssao;
}

SsaoTexs::SsaoTexs() :
    mDevice(nullptr),
    mDeviceContext(nullptr),
    mNormalTexture(nullptr),
    mDepthTexture(nullptr),
    mSsaoTexture(nullptr),
    mNormalRenderTargetView(nullptr),
    mSsaoRenderTargetView(nullptr),
    mDepthStencilView(nullptr),
    mNormalShaderResourceView(nullptr),
    mSsaoShaderResourceView(nullptr),
    mDepthShaderResourceView(nullptr),
    mSsaoConstantBuffer(nullptr),
    mSsaoVertexBuffer(nullptr),
    mSsaoIndexBuffer(nullptr)
{
    g_Ssao = this;

    g_SSAOcb.mProj = DirectX::XMMatrixTranspose(
        TEMP::GetProjMat());
    DirectX::XMVECTOR det =
        DirectX::XMMatrixDeterminant(g_SSAOcb.mProj);
    g_SSAOcb.mInvProj = DirectX::XMMatrixTranspose(
        DirectX::XMMatrixInverse(&det, g_SSAOcb.mProj));
}

bool SsaoTexs::Init(
    ID3D11Device* _device,
    ID3D11DeviceContext* _deviceContext,
    UINT _width, UINT _height)
{
    mDevice = _device;
    mDeviceContext = _deviceContext;

    HRESULT hr = S_OK;
    D3D11_TEXTURE2D_DESC texDesc = {};
    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    D3D11_BUFFER_DESC bufDesc = {};
    ZeroMemory(&texDesc, sizeof(texDesc));
    ZeroMemory(&rtvDesc, sizeof(rtvDesc));
    ZeroMemory(&dsvDesc, sizeof(dsvDesc));
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    ZeroMemory(&bufDesc, sizeof(bufDesc));

    texDesc.Width = _width;
    texDesc.Height = _height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;
    texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    texDesc.BindFlags =
        D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    hr = mDevice->CreateTexture2D(
        &texDesc, nullptr, &mNormalTexture);
    if (FAILED(hr))
    {
        return false;
    }

    rtvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    rtvDesc.Texture2D.MipSlice = 0;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    hr = mDevice->CreateRenderTargetView(
        mNormalTexture, &rtvDesc, &mNormalRenderTargetView);
    if (FAILED(hr))
    {
        return false;
    }

    srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    hr = mDevice->CreateShaderResourceView(mNormalTexture,
        &srvDesc, &mNormalShaderResourceView);
    if (FAILED(hr))
    {
        return false;
    }

    texDesc.Width = _width;
    texDesc.Height = _height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;
    texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    texDesc.BindFlags =
        D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    hr = mDevice->CreateTexture2D(
        &texDesc, nullptr, &mDepthTexture);
    if (FAILED(hr))
    {
        return false;
    }

    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;
    hr = mDevice->CreateDepthStencilView(
        mDepthTexture, &dsvDesc, &mDepthStencilView);
    if (FAILED(hr))
    {
        return false;
    }

    srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    hr = mDevice->CreateShaderResourceView(mDepthTexture,
        &srvDesc, &mDepthShaderResourceView);
    if (FAILED(hr))
    {
        return false;
    }

    texDesc.Width = _width / 2;
    texDesc.Height = _height / 2;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.BindFlags =
        D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    hr = mDevice->CreateTexture2D(
        &texDesc, nullptr, &mSsaoTexture);
    if (FAILED(hr))
    {
        return false;
    }

    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvDesc.Texture2D.MipSlice = 0;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    hr = mDevice->CreateRenderTargetView(
        mSsaoTexture, &rtvDesc, &mSsaoRenderTargetView);
    if (FAILED(hr))
    {
        return false;
    }

    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    hr = mDevice->CreateShaderResourceView(mSsaoTexture,
        &srvDesc, &mSsaoShaderResourceView);
    if (FAILED(hr))
    {
        return false;
    }

    bufDesc.Usage = D3D11_USAGE_DEFAULT;
    bufDesc.ByteWidth = sizeof(SSAOConstantBuffer);
    bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufDesc.CPUAccessFlags = 0;
    hr = mDevice->CreateBuffer(
        &bufDesc, nullptr, &mSsaoConstantBuffer);
    if (FAILED(hr))
    {
        return false;
    }

    MESH_VERTEX v[4] = {};
    v[0].Position = DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f);
    v[1].Position = DirectX::XMFLOAT3(-1.0f, +1.0f, 0.0f);
    v[2].Position = DirectX::XMFLOAT3(+1.0f, +1.0f, 0.0f);
    v[3].Position = DirectX::XMFLOAT3(+1.0f, -1.0f, 0.0f);
    v[0].Normal = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    v[1].Normal = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
    v[2].Normal = DirectX::XMFLOAT3(2.0f, 0.0f, 0.0f);
    v[3].Normal = DirectX::XMFLOAT3(3.0f, 0.0f, 0.0f);
    v[0].TexCoord = DirectX::XMFLOAT2(0.0f, 1.0f);
    v[1].TexCoord = DirectX::XMFLOAT2(0.0f, 0.0f);
    v[2].TexCoord = DirectX::XMFLOAT2(1.0f, 0.0f);
    v[3].TexCoord = DirectX::XMFLOAT2(1.0f, 1.0f);
    bufDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufDesc.ByteWidth = sizeof(MESH_VERTEX) * 4;
    bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufDesc.CPUAccessFlags = 0;
    bufDesc.MiscFlags = 0;
    bufDesc.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA vinitData = {};
    ZeroMemory(&vinitData, sizeof(vinitData));
    vinitData.pSysMem = v;
    hr = mDevice->CreateBuffer(
        &bufDesc, &vinitData, &mSsaoVertexBuffer);
    if (FAILED(hr))
    {
        return false;
    }

    UINT indices[6] =
    {
        0, 1, 2,
        0, 2, 3
    };
    bufDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufDesc.ByteWidth = sizeof(UINT) * 6;
    bufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufDesc.CPUAccessFlags = 0;
    bufDesc.StructureByteStride = 0;
    bufDesc.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData = {};
    ZeroMemory(&iinitData, sizeof(iinitData));
    iinitData.pSysMem = indices;
    hr = mDevice->CreateBuffer(
        &bufDesc, &iinitData, &mSsaoIndexBuffer);
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

void SsaoTexs::ClearAndStop()
{
    if (mDepthShaderResourceView)
    {
        mDepthShaderResourceView->Release();
    }
    if (mDepthStencilView)
    {
        mDepthStencilView->Release();
    }
    if (mDepthTexture)
    {
        mDepthTexture->Release();
    }
    if (mNormalShaderResourceView)
    {
        mNormalShaderResourceView->Release();
    }
    if (mNormalRenderTargetView)
    {
        mNormalRenderTargetView->Release();
    }
    if (mNormalTexture)
    {
        mNormalTexture->Release();
    }
    if (mSsaoShaderResourceView)
    {
        mSsaoShaderResourceView->Release();
    }
    if (mSsaoRenderTargetView)
    {
        mSsaoRenderTargetView->Release();
    }
    if (mSsaoTexture)
    {
        mSsaoTexture->Release();
    }
    if (mSsaoConstantBuffer)
    {
        mSsaoConstantBuffer->Release();
    }
    if (mSsaoVertexBuffer)
    {
        mSsaoVertexBuffer->Release();
    }
    if (mSsaoIndexBuffer)
    {
        mSsaoIndexBuffer->Release();
    }
}

void SsaoTexs::SetNormalRenderTarget()
{
    mDeviceContext->OMSetRenderTargets(1,
        &mNormalRenderTargetView, mDepthStencilView);
    mDeviceContext->ClearDepthStencilView(
        mDepthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);
}

void SsaoTexs::SetSsaoRenderTarget()
{
    mDeviceContext->OMSetRenderTargets(1,
        &mSsaoRenderTargetView, nullptr);
    UINT stride = sizeof(MESH_VERTEX);
    UINT offset = 0;
    mDeviceContext->IASetVertexBuffers(0, 1, &mSsaoVertexBuffer,
        &stride, &offset);
    mDeviceContext->IASetIndexBuffer(mSsaoIndexBuffer,
        DXGI_FORMAT_R32_UINT, 0);
    mDeviceContext->UpdateSubresource(
        mSsaoConstantBuffer, 0, nullptr, &g_SSAOcb, 0, 0);
    mDeviceContext->VSSetConstantBuffers(
        0, 1, &mSsaoConstantBuffer);
    mDeviceContext->PSSetConstantBuffers(
        0, 1, &mSsaoConstantBuffer);
    mDeviceContext->DrawIndexed(6, 0, 0);
}
