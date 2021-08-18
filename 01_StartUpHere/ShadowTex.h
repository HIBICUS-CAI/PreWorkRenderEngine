#pragma once

#include <d3d11_1.h>

class ShadowTex* GetShadow();

class ShadowTex
{
public:
    ShadowTex();
    ~ShadowTex() {}

    bool Init(
        ID3D11Device* _device,
        ID3D11DeviceContext* _deviceContext,
        UINT _width, UINT _height);
    void ClearAndStop();
    void SetRenderTarget();
    void ClearRenderTarget();
    ID3D11ShaderResourceView* GetSRV();
    void UnBoundDSV();

private:
    ID3D11Device* mDevice;
    ID3D11DeviceContext* mDeviceContext;
    ID3D11Texture2D* mDepthStencilTexture;
    ID3D11DepthStencilView* mDepthStencilView;
    ID3D11ShaderResourceView* mShaderResourceView;
};
