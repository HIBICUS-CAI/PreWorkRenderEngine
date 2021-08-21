#include "SsaoTexs.h"
#include <Windows.h>

SsaoTexs* g_Ssao = nullptr;

SsaoTexs* GetSsao()
{
    return g_Ssao;
}

SsaoTexs::SsaoTexs() :
    mDevice(nullptr),
    mDeviceContext(nullptr),
    mNormalTexture(nullptr),
    mDepthTexture(nullptr),
    mNormalRenderTargetView(nullptr),
    mDepthStencilView(nullptr),
    mNormalShaderResourceView(nullptr),
    mDepthShaderResourceView(nullptr)
{
    g_Ssao = this;
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
    ZeroMemory(&texDesc, sizeof(texDesc));
    ZeroMemory(&rtvDesc, sizeof(rtvDesc));
    ZeroMemory(&dsvDesc, sizeof(dsvDesc));
    ZeroMemory(&srvDesc, sizeof(srvDesc));

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
}

void SsaoTexs::SetNormalRenderTarget()
{
    mDeviceContext->OMSetRenderTargets(1,
        &mNormalRenderTargetView, mDepthStencilView);
    mDeviceContext->ClearDepthStencilView(
        mDepthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);
}
