#pragma once

#include <Windows.h>
#include <d3d11_1.h>

#define CHECK_RELEASE
#define SHOW_CUBE

namespace TEMP
{
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

    HRESULT PrepareCube(HWND wndHandle);
}
