//---------------------------------------------------------------
// File: RSDevices.h
// Proj: RenderSystem_DX11
// Info: 保存并提供与DirectX直接相关的内容和引用
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#pragma once

#include "RSCommon.h"
#include <Windows.h>
#include <d3d11_1.h>

class RSDevices
{
public:
    RSDevices();
    ~RSDevices();

    bool StartUp(class RSRoot_DX11* _root, HWND _wnd);
    void CleanAndStop();

    ID3D11Device* GetDevice() const;
    ID3D11DeviceContext* GetSTContext() const;

    void PresentSwapChain();

private:
    bool CreateDevices(HWND _wnd, UINT _width, UINT _height);
    void ApplyViewPort();

private:
    class RSRoot_DX11* mRootPtr;

    D3D_DRIVER_TYPE mDriveType;
    D3D_FEATURE_LEVEL mFeatureLevel;

    ID3D11Device* mDevice;
    ID3D11DeviceContext* mImmediateContext;
    ID3D11Device1* mDevice1;
    ID3D11DeviceContext1* mImmediateContext1;

    IDXGISwapChain* mDXGISwapChain;
    IDXGISwapChain1* mDXGISwapChain1;
    ID3D11RenderTargetView* mSwapChainRtv;
    D3D11_VIEWPORT mFullWindowViewPort;
};
