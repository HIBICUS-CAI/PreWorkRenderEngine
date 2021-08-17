#pragma once

#include <d3d11.h>

class ShadowMap
{
public:
    ShadowMap();
    ~ShadowMap() {}

    bool Init(
        ID3D11Device* _device,
        ID3D11DeviceContext* _deviceContext,
        UINT _width, UINT _height);
    void ClearAndStop();
    void SetRenderTarget();
    void ClearRenderTarget(
        float _r, float _g, float _b, float _a);
    ID3D11ShaderResourceView* GetSRV();

private:
    ID3D11Device* mDevice;
    ID3D11DeviceContext* mDeviceContext;
    ID3D11Texture2D* mRenderTargetTexture;
    ID3D11Texture2D* mDepthStencilTexture;
    ID3D11RenderTargetView* mRenderTargetView;
    ID3D11DepthStencilView* mDepthStencilView;
    ID3D11ShaderResourceView* mShaderResourceView;
};
