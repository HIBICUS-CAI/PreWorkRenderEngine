#pragma once

#include <Windows.h>
#include <d3d11_1.h>

#define CHECK_RELEASE
//#define SHOW_CUBE

namespace TEMP
{
    IDXGISwapChain* GetSwapChain();
    ID3D11Device* GetD3DDevicePointer();
    ID3D11DeviceContext* GetD3DDevContPointer();
    D3D_DRIVER_TYPE GetDriverType();
    void TempRenderBegin();
    void TempRenderEnd();

    //------------------------
    void setLightOffsetX(FLOAT x);
    void setLightOffsetY(FLOAT y);
    void setLightOffsetZ(FLOAT z);
    //------------------------

    HRESULT InitD3D11Device(HWND wndHandle);

    void ChangeWindowSize(HWND wndHandle);

    void CleanupDevice();

    void Render();

    void RenderCube();

    HRESULT CompileShaderFromFile(const WCHAR* szFileName,
        LPCSTR szEntryPoint, LPCSTR szShaderModel,
        ID3DBlob** ppBlobOut);

    HRESULT PrepareBasicRender(HWND wndHandle);
}
