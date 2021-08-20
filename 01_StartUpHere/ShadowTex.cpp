#include "ShadowTex.h"
#include <Windows.h>

ShadowTex* g_Shadow = nullptr;
ID3D11RasterizerState* g_ShadowRS = nullptr;
ID3D11RasterizerState* g_NormalRS = nullptr;

ShadowTex* GetShadow()
{
    return g_Shadow;
}

ShadowTex::ShadowTex() :
    mDevice(nullptr),
    mDeviceContext(nullptr),
    mDepthStencilTexture(nullptr),
    mDepthStencilView(nullptr),
    mShaderResourceView(nullptr)
{
    g_Shadow = this;
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
    D3D11_RASTERIZER_DESC shadowRasterDesc = {};
    D3D11_RASTERIZER_DESC normalRasterDesc = {};
    ZeroMemory(&texDesc, sizeof(texDesc));
    ZeroMemory(&dsvDesc, sizeof(dsvDesc));
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    ZeroMemory(&shadowRasterDesc, sizeof(shadowRasterDesc));
    ZeroMemory(&normalRasterDesc, sizeof(normalRasterDesc));

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

    normalRasterDesc.FillMode = D3D11_FILL_SOLID;
    normalRasterDesc.CullMode = D3D11_CULL_BACK;
    normalRasterDesc.FrontCounterClockwise = FALSE;
    normalRasterDesc.DepthBias = 0;
    normalRasterDesc.SlopeScaledDepthBias = 0.f;
    normalRasterDesc.DepthBiasClamp = 0.f;
    normalRasterDesc.DepthClipEnable = TRUE;
    normalRasterDesc.ScissorEnable = FALSE;
    normalRasterDesc.MultisampleEnable = TRUE;
    normalRasterDesc.AntialiasedLineEnable = TRUE;

    hr = mDevice->CreateRasterizerState(&normalRasterDesc,
        &g_NormalRS);
    if (FAILED(hr))
    {
        return false;
    }

    shadowRasterDesc.FillMode = D3D11_FILL_SOLID;
    shadowRasterDesc.CullMode = D3D11_CULL_BACK;
    shadowRasterDesc.FrontCounterClockwise = FALSE;
    shadowRasterDesc.DepthBias = 100000;
    //shadowRasterDesc.DepthBias = 600000;
    shadowRasterDesc.SlopeScaledDepthBias = 1.f;
    shadowRasterDesc.DepthBiasClamp = 0.f;
    shadowRasterDesc.DepthClipEnable = TRUE;
    shadowRasterDesc.ScissorEnable = FALSE;
    shadowRasterDesc.MultisampleEnable = FALSE;
    shadowRasterDesc.AntialiasedLineEnable = FALSE;

    hr = mDevice->CreateRasterizerState(&shadowRasterDesc,
        &g_ShadowRS);
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

    if (g_NormalRS)
    {
        g_NormalRS->Release();
    }

    if (g_ShadowRS)
    {
        g_ShadowRS->Release();
    }
}

void ShadowTex::SetRenderTarget()
{
    mDeviceContext->RSSetState(g_ShadowRS);
    mDeviceContext->OMSetRenderTargets(0,
        nullptr, mDepthStencilView);
}

void ShadowTex::ClearRenderTarget()
{
    mDeviceContext->ClearDepthStencilView(
        mDepthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);
}

void ShadowTex::UnBoundDSV()
{
    static ID3D11RenderTargetView* nullRTV = nullptr;
    mDeviceContext->OMSetRenderTargets(0,
        &nullRTV, nullptr);
    mDeviceContext->RSSetState(g_NormalRS);
}

ID3D11ShaderResourceView* ShadowTex::GetSRV()
{
    return mShaderResourceView;
}
