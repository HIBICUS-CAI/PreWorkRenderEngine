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
    void SetSsaoRenderTarget();
    ID3D11ShaderResourceView* GetSsaoMap();
    void RunHBlurComputeShader();
    void RunVBlurComputeShader();
    //void ClearNormalRenderTarget();
    //ID3D11ShaderResourceView* GetNormalSRV();

private:
    bool BuildRandomTexture();
    void BuildOffsetVectors();

private:
    ID3D11Device* mDevice;
    ID3D11DeviceContext* mDeviceContext;
    ID3D11Texture2D* mNormalTexture;
    ID3D11Texture2D* mDepthTexture;
    ID3D11Texture2D* mSsaoTexture;
    ID3D11Texture2D* mRandomTexture;
    ID3D11RenderTargetView* mNormalRenderTargetView;
    ID3D11RenderTargetView* mSsaoRenderTargetView;
    ID3D11DepthStencilView* mDepthStencilView;
    ID3D11ShaderResourceView* mNormalShaderResourceView;
    ID3D11ShaderResourceView* mSsaoShaderResourceView;
    ID3D11ShaderResourceView* mDepthShaderResourceView;
    ID3D11ShaderResourceView* mRandomShaderResourceView;
    ID3D11UnorderedAccessView* mSsaoUnorderedAccessView;

    ID3D11Buffer* mSsaoConstantBuffer;
    ID3D11Buffer* mSsaoVertexBuffer;
    ID3D11Buffer* mSsaoIndexBuffer;

    ID3D11SamplerState* mSamplePointClamp;
    ID3D11SamplerState* mSampleLinearClamp;
    ID3D11SamplerState* mSampleDepthMap;
    ID3D11SamplerState* mSampleLinearWrap;
};
