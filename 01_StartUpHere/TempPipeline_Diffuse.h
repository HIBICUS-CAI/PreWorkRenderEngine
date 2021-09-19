#pragma once

#include <string>
#include <vector>
#include "RSPass_Base.h"

struct TEMP_WVPBuffer
{
    DirectX::XMFLOAT4X4 mView = {};
    DirectX::XMFLOAT4X4 mProjection = {};
};

void PassRootToTempPipeline(class RSRoot_DX11* _root);

bool CreateTempPipeline();
void ExecuateTempPipeline();
void ReleaseTempPipeline();

class RSPass_Diffuse :public RSPass_Base
{
public:
    RSPass_Diffuse(std::string& _name, PASS_TYPE _type,
        class RSRoot_DX11* _root);
    RSPass_Diffuse(const RSPass_Diffuse& _source);
    virtual ~RSPass_Diffuse();

public:
    virtual RSPass_Diffuse* ClonePass() override;

    virtual bool InitPass();

    virtual void ReleasePass();

    virtual void ExecuatePass();

private:
    bool CreateShaders();
    bool CreateStates();
    bool CreateViews();
    bool CreateSamplers();
    bool CreateBuffers();

private:
    ID3D11VertexShader* mVertexShader;
    ID3D11PixelShader* mPixelShader;
    //ID3D11RasterizerState* mRasterizerState;
    //ID3D11DepthStencilState* mDepthStencilState;
    ID3D11RenderTargetView* mRenderTargetView;
    ID3D11DepthStencilView* mDepthStencilView;
    ID3D11SamplerState* mSampler;
    DRAWCALL_TYPE mDrawCallType;
    RSDrawCallsPipe* mDrawCallPipe;
    ID3D11Buffer* mWVPBuffer;
    TEMP_WVPBuffer mCPUBuffer;
    ID3D11Buffer* mStructedBuffer;
    ID3D11ShaderResourceView* mStructedBufferSrv;
};

class RSPass_FromTex :public RSPass_Base
{
public:
    RSPass_FromTex(std::string& _name, PASS_TYPE _type,
        class RSRoot_DX11* _root);
    RSPass_FromTex(const RSPass_FromTex& _source);
    virtual ~RSPass_FromTex();

public:
    virtual RSPass_FromTex* ClonePass() override;

    virtual bool InitPass();

    virtual void ReleasePass();

    virtual void ExecuatePass();

private:
    bool CreateBuffer();
    bool CreateShaders();
    bool CreateViews();
    bool CreateSamplers();

private:
    ID3D11Buffer* mIndexBuffer;
    D3D_PRIMITIVE_TOPOLOGY mTopology;
    ID3D11InputLayout* mLayout;
    ID3D11VertexShader* mVertexShader;
    ID3D11PixelShader* mPixelShader;
    //ID3D11RasterizerState* mRasterizerState;
    //ID3D11DepthStencilState* mDepthStencilState;
    ID3D11RenderTargetView* mSwapChainRtv;
    ID3D11ShaderResourceView* mInputSrv;
    ID3D11SamplerState* mSampler;
};
