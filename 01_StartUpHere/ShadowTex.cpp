#include "ShadowTex.h"
#include <Windows.h>

ShadowTex::ShadowTex() :
    mDevice(nullptr),
    mDeviceContext(nullptr),
    mDepthStencilTexture(nullptr),
    mDepthStencilView(nullptr),
    mShaderResourceView(nullptr)
{

}

bool ShadowTex::Init(
    ID3D11Device* _device,
    ID3D11DeviceContext* _deviceContext,
    UINT _width, UINT _height)
{
    mDevice = _device;
    mDeviceContext = _deviceContext;

    HRESULT hr = S_OK;
    D3D11_TEXTURE2D_DESC texDesc = {};
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    ZeroMemory(&texDesc, sizeof(texDesc));
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
    texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    texDesc.BindFlags = 
        D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    hr = mDevice->CreateTexture2D(
        &texDesc, nullptr, &mDepthStencilTexture);
    if (FAILED(hr))
    {
        return false;
    }

    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;

    hr = mDevice->CreateDepthStencilView(
        mDepthStencilTexture, &dsvDesc, &mDepthStencilView);
    if (FAILED(hr))
    {
        return false;
    }

    srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    hr = mDevice->CreateShaderResourceView(mDepthStencilTexture,
        &srvDesc, &mShaderResourceView);
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

void ShadowTex::ClearAndStop()
{
    if (mShaderResourceView)
    {
        mShaderResourceView->Release();
        mShaderResourceView = nullptr;
    }

    if (mDepthStencilView)
    {
        mDepthStencilView->Release();
        mDepthStencilView = nullptr;
    }

    if (mDepthStencilTexture)
    {
        mDepthStencilTexture->Release();
        mDepthStencilTexture = nullptr;
    }
}

void ShadowTex::SetRenderTarget()
{
    mDeviceContext->OMSetRenderTargets(0,
        nullptr, mDepthStencilView);
}

void ShadowTex::ClearRenderTarget(
    float _r, float _g, float _b, float _a)
{
    mDeviceContext->ClearDepthStencilView(
        mDepthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);
}

ID3D11ShaderResourceView* ShadowTex::GetSRV()
{
    return mShaderResourceView;
}
