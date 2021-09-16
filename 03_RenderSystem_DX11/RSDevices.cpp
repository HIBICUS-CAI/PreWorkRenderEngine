//---------------------------------------------------------------
// File: RSDevices.cpp
// Proj: RenderSystem_DX11
// Info: 保存并提供与DirectX直接相关的内容和引用
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#include "RSDevices.h"
#include "RSRoot_DX11.h"

RSDevices::RSDevices() :
    mRootPtr(nullptr),
    mDriveType(D3D_DRIVER_TYPE_HARDWARE),
    mFeatureLevel(D3D_FEATURE_LEVEL_11_1),
    mDevice(nullptr), mImmediateContext(nullptr),
    mDevice1(nullptr), mImmediateContext1(nullptr),
    mDXGISwapChain(nullptr), mDXGISwapChain1(nullptr),
    mSwapChainRtv(nullptr), mFullWindowViewPort({})
{

}

RSDevices::~RSDevices()
{

}

bool RSDevices::StartUp(RSRoot_DX11* _root, HWND _wnd)
{
    if (!_root) { return false; }

    mRootPtr = _root;

    RECT wndRect = {};
    GetClientRect(_wnd, &wndRect);
    UINT wndWidth = wndRect.right - wndRect.left;
    UINT wndHeight = wndRect.bottom - wndRect.top;

    mFullWindowViewPort.Width = (FLOAT)wndWidth;
    mFullWindowViewPort.Height = (FLOAT)wndHeight;
    mFullWindowViewPort.MinDepth = 0.f;
    mFullWindowViewPort.MaxDepth = 1.f;
    mFullWindowViewPort.TopLeftX = 0.f;
    mFullWindowViewPort.TopLeftY = 0.f;

    if (!CreateDevices(_wnd, wndWidth, wndHeight))
    {
        return false;
    }

    ApplyViewPort();

    return true;
}

void RSDevices::CleanAndStop()
{
    if (mImmediateContext)
    {
        mImmediateContext->ClearState();
    }
    SAFE_RELEASE(mSwapChainRtv);
    SAFE_RELEASE(mImmediateContext1);
    SAFE_RELEASE(mImmediateContext);
    SAFE_RELEASE(mDXGISwapChain1);
    SAFE_RELEASE(mDXGISwapChain);

#ifdef _DEBUG
    ID3D11Debug* pDebug = nullptr;
    HRESULT hr = mDevice->QueryInterface(
        IID_PPV_ARGS(&pDebug));
    if (SUCCEEDED(hr))
    {
        hr = pDebug->ReportLiveDeviceObjects(
            D3D11_RLDO_DETAIL);
        pDebug->Release();
    }
#endif // _DEBUG

    SAFE_RELEASE(mDevice1);
    SAFE_RELEASE(mDevice);
}

ID3D11Device* RSDevices::GetDevice() const
{
    return mDevice;
}

ID3D11DeviceContext* RSDevices::GetSTContext() const
{
    return mImmediateContext;
}

ID3D11RenderTargetView* RSDevices::GetSwapChainRtv() const
{
    return mSwapChainRtv;
}

void RSDevices::PresentSwapChain()
{
    mDXGISwapChain->Present(0, 0);
}

bool RSDevices::CreateDevices(HWND _wnd,
    UINT _width, UINT _height)
{
    HRESULT hr = S_OK;

    UINT deviceCreateFlag = 0;
#ifdef _DEBUG
    deviceCreateFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_REFERENCE,
        D3D_DRIVER_TYPE_WARP
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);
    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0
    };
    UINT numFeatLevels = ARRAYSIZE(featureLevels);
    mFeatureLevel = featureLevels[0];

    for (UINT i = 0; i < numDriverTypes; i++)
    {
        mDriveType = driverTypes[i];
        hr = D3D11CreateDevice(nullptr, mDriveType,
            nullptr, deviceCreateFlag, featureLevels,
            numFeatLevels, D3D11_SDK_VERSION,
            &mDevice, &mFeatureLevel,
            &mImmediateContext);

        if (hr == E_INVALIDARG)
        {
            hr = D3D11CreateDevice(nullptr, mDriveType,
                nullptr, deviceCreateFlag,
                featureLevels + 1, numFeatLevels - 1,
                D3D11_SDK_VERSION, &mDevice,
                &mFeatureLevel, &mImmediateContext);
        }

        if (SUCCEEDED(hr))
        {
            break;
        }
    }
    FAIL_HR_RETURN(hr);

    IDXGIFactory1* dxgiFactory1 = nullptr;
    {
        IDXGIDevice* dxgiDevice = nullptr;
        hr = mDevice->QueryInterface(
            IID_PPV_ARGS(&dxgiDevice));
        if (SUCCEEDED(hr))
        {
            IDXGIAdapter* adapter = nullptr;
            hr = dxgiDevice->GetAdapter(&adapter);
            if (SUCCEEDED(hr))
            {
                hr = adapter->GetParent(
                    IID_PPV_ARGS(&dxgiFactory1));
                adapter->Release();
            }
            dxgiDevice->Release();
        }
    }
    FAIL_HR_RETURN(hr);

    IDXGIFactory2* dxgiFactory2 = nullptr;
    hr = dxgiFactory1->QueryInterface(
        IID_PPV_ARGS(&dxgiFactory2));
    if (dxgiFactory2)
    {
        // 11.1+
        hr = mDevice->QueryInterface(
            IID_PPV_ARGS(&mDevice1));
        if (SUCCEEDED(hr))
        {
            mImmediateContext->QueryInterface(
                IID_PPV_ARGS(&mImmediateContext1));
        }
        DXGI_SWAP_CHAIN_DESC1 dc = {};
        dc.Width = _width;
        dc.Height = _height;
        dc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        dc.SampleDesc.Count = 1;
        dc.SampleDesc.Quality = 0;
        dc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        dc.BufferCount = 2;
        dc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        hr = dxgiFactory2->CreateSwapChainForHwnd(
            mDevice, _wnd,
            &dc, nullptr, nullptr, &mDXGISwapChain1);
        if (SUCCEEDED(hr))
        {
            hr = mDXGISwapChain1->QueryInterface(
                IID_PPV_ARGS(&mDXGISwapChain));
        }

        dxgiFactory2->Release();
    }
    else
    {
        // 11.0
        DXGI_SWAP_CHAIN_DESC dc = {};
        dc.BufferCount = 2;
        dc.BufferDesc.Width = _width;
        dc.BufferDesc.Height = _height;
        dc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        dc.BufferDesc.RefreshRate.Numerator = 60;
        dc.BufferDesc.RefreshRate.Denominator = 1;
        dc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        dc.OutputWindow = _wnd;
        dc.SampleDesc.Count = 1;
        dc.SampleDesc.Quality = 0;
        dc.Windowed = TRUE;
        dc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        hr = dxgiFactory1->CreateSwapChain(
            mDevice, &dc, &mDXGISwapChain);
    }

    dxgiFactory1->Release();
    FAIL_HR_RETURN(hr);

    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = mDXGISwapChain->GetBuffer(
        0, IID_PPV_ARGS(&pBackBuffer));
    FAIL_HR_RETURN(hr);

    hr = mDevice->CreateRenderTargetView(
        pBackBuffer, nullptr, &mSwapChainRtv);
    pBackBuffer->Release();
    FAIL_HR_RETURN(hr);

    return true;
}

void RSDevices::ApplyViewPort()
{
    mImmediateContext->RSSetViewports(1, &mFullWindowViewPort);
}
