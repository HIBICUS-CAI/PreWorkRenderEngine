#pragma once

#include <string>
#include <vector>
#include "RSPass_Base.h"

struct ViewProj
{
    DirectX::XMFLOAT4X4 mViewMat = {};
    DirectX::XMFLOAT4X4 mProjMat = {};
};

struct Ambient
{
    DirectX::XMFLOAT4 mAmbient = {};
};

struct LightInfo
{
    DirectX::XMFLOAT3 mCameraPos = {};
    float mPad0 = 0.f;
    UINT mDirectLightNum = 0;
    UINT mSpotLightNum = 0;
    UINT mPointLightNum = 0;
    UINT mPad1 = 0;
};

void PassRootToTempLightPipeline(class RSRoot_DX11* _root);

bool CreateTempLightPipeline();

class RSPass_Light :public RSPass_Base
{
public:
    RSPass_Light(std::string& _name, PASS_TYPE _type,
        class RSRoot_DX11* _root);
    RSPass_Light(const RSPass_Light& _source);
    virtual ~RSPass_Light();

public:
    virtual RSPass_Light* ClonePass() override;

    virtual bool InitPass();

    virtual void ReleasePass();

    virtual void ExecuatePass();

private:
    bool CreateShaders();
    bool CreateStates();
    bool CreateBuffers();
    bool CreateViews();
    bool CreateSamplers();

private:
    ID3D11VertexShader* mVertexShader;
    ID3D11PixelShader* mPixelShader;
    ID3D11RasterizerState* mRasterizerState;
    ID3D11RenderTargetView* mRenderTargetView;
    ID3D11DepthStencilView* mDepthStencilView;
    ID3D11SamplerState* mMeshTexSampler;
    DRAWCALL_TYPE mDrawCallType;
    RSDrawCallsPipe* mDrawCallPipe;
    ID3D11Buffer* mViewProjStructedBuffer;
    ID3D11ShaderResourceView* mViewProjStructedBufferSrv;
    ID3D11Buffer* mInstanceStructedBuffer;
    ID3D11ShaderResourceView* mInstanceStructedBufferSrv;
    ID3D11Buffer* mLightInfoStructedBuffer;
    ID3D11ShaderResourceView* mLightInfoStructedBufferSrv;
    ID3D11Buffer* mLightStructedBuffer;
    ID3D11ShaderResourceView* mLightStructedBufferSrv;
    ID3D11Buffer* mAmbientStructedBuffer;
    ID3D11ShaderResourceView* mAmbientStructedBufferSrv;
    ID3D11Buffer* mMaterialStructedBuffer;
    ID3D11ShaderResourceView* mMaterialStructedBufferSrv;
};

class RSPass_Shadow :public RSPass_Base
{
public:
    RSPass_Shadow(std::string& _name, PASS_TYPE _type,
        class RSRoot_DX11* _root);
    RSPass_Shadow(const RSPass_Shadow& _source);
    virtual ~RSPass_Shadow();

public:
    virtual RSPass_Shadow* ClonePass() override;

    virtual bool InitPass();

    virtual void ReleasePass();

    virtual void ExecuatePass();

private:
    bool CreateShaders();
    bool CreateStates();
    bool CreateBuffers();
    bool CreateViews();
    bool CreateSamplers();

private:
    ID3D11VertexShader* mVertexShader;
    ID3D11RasterizerState* mRasterizerState;
    ID3D11DepthStencilView* mDepthStencilView;
    DRAWCALL_TYPE mDrawCallType;
    RSDrawCallsPipe* mDrawCallPipe;
    ID3D11Buffer* mViewProjStructedBuffer;
    ID3D11ShaderResourceView* mViewProjStructedBufferSrv;
    ID3D11Buffer* mInstanceStructedBuffer;
    ID3D11ShaderResourceView* mInstanceStructedBufferSrv;
};
