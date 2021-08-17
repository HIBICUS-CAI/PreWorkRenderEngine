#include "ShadowMap.h"
#include <Windows.h>

ShadowMap::ShadowMap() :
    mDevice(nullptr),
    mDeviceContext(nullptr),
    mRenderTargetTexture(nullptr),
    mDepthStencilTexture(nullptr),
    mRenderTargetView(nullptr),
    mDepthStencilView(nullptr),
    mShaderResourceView(nullptr)
{

}

bool ShadowMap::Init(
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
    texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags =
        D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;

    hr = mDevice->CreateTexture2D(&texDesc, nullptr,
        &mRenderTargetTexture);
    if (FAILED(hr))
    {
        return false;
    }

    rtvDesc.Format = texDesc.Format;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;

    hr = mDevice->CreateRenderTargetView(mRenderTargetTexture,
        &rtvDesc, &mRenderTargetView);
    if (FAILED(hr))
    {
        return false;
    }

    srvDesc.Format = texDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    hr = mDevice->CreateShaderResourceView(mRenderTargetTexture,
        &srvDesc, &mShaderResourceView);
    if (FAILED(hr))
    {
        return false;
    }

    texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    hr = mDevice->CreateTexture2D(
        &texDesc, nullptr, &mDepthStencilTexture);
    if (FAILED(hr))
    {
        return false;
    }

    dsvDesc.Format = texDesc.Format;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;

    hr = mDevice->CreateDepthStencilView(
        mDepthStencilTexture, &dsvDesc, &mDepthStencilView);
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

void ShadowMap::ClearAndStop()
{
    if (mShaderResourceView)
    {
        mShaderResourceView->Release();
        mShaderResourceView = nullptr;
    }

    if (mRenderTargetView)
    {
        mRenderTargetView->Release();
        mRenderTargetView = nullptr;
    }

    if (mRenderTargetTexture)
    {
        mRenderTargetTexture->Release();
        mRenderTargetTexture = nullptr;
    }
}

void ShadowMap::SetRenderTarget()
{
    mDeviceContext->OMSetRenderTargets(1,
        &mRenderTargetView, mDepthStencilView);
}

void ShadowMap::ClearRenderTarget(
    float _r, float _g, float _b, float _a)
{
    static FLOAT color[4] = {};
    color[0] = _r;
    color[1] = _g;
    color[2] = _b;
    color[3] = _a;
    mDeviceContext->ClearRenderTargetView(
        mRenderTargetView, color);
    mDeviceContext->ClearDepthStencilView(
        mDepthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);
}

ID3D11ShaderResourceView* ShadowMap::GetSRV()
{
    return mShaderResourceView;
}
