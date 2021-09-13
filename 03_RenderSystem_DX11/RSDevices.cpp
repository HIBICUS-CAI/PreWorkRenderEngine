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
    mDXGISwapChain(nullptr), mSwapChainRtv(nullptr),
    mFullWindowViewPort({})
{

}

RSDevices::~RSDevices()
{

}

bool RSDevices::StartUp(RSRoot_DX11* _root, HWND _wnd)
{
    // TEMP----------------------
    return true;
    // TEMP----------------------
}

void RSDevices::CleanAndStop()
{

}

ID3D11Device* RSDevices::GetDevice() const
{
    return mDevice;
}

ID3D11DeviceContext* RSDevices::GetSTContext() const
{
    return mImmediateContext;
}

void RSDevices::PresentSwapChain()
{

}

bool RSDevices::CreateDevices(HWND _wnd,
    UINT _width, UINT _height)
{
    // TEMP----------------------
    return true;
    // TEMP----------------------
}

void RSDevices::ApplyViewPort()
{

}
