#pragma once

#include <d3d11_1.h>

class SsaoTexs* GetSsao();

class SsaoTexs
{
public:
    SsaoTexs();
    ~SsaoTexs() {}

    bool Init(
        ID3D11Device* _device,
        ID3D11DeviceContext* _deviceContext,
        UINT _width, UINT _height);
    void ClearAndStop();
    void SetNormalRenderTarget();
    //void ClearNormalRenderTarget();
    //ID3D11ShaderResourceView* GetNormalSRV();

private:
    ID3D11Device* mDevice;
    ID3D11DeviceContext* mDeviceContext;
    ID3D11Texture2D* mNormalTexture;
    ID3D11Texture2D* mDepthTexture;
    ID3D11RenderTargetView* mNormalRenderTargetView;
    ID3D11DepthStencilView* mDepthStencilView;
    ID3D11ShaderResourceView* mNormalShaderResourceView;
    ID3D11ShaderResourceView* mDepthShaderResourceView;
};
