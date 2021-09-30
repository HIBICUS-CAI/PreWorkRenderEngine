#pragma once

#include <string>
#include <vector>
#include "RSPass_Base.h"
#include <DirectXTK\SpriteFont.h>

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

struct ShadowInfo
{
    DirectX::XMFLOAT4X4 mShadowViewMat = {};
    DirectX::XMFLOAT4X4 mShadowProjMat = {};
    DirectX::XMFLOAT4X4 mSSAOMat = {};
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
    ID3D11SamplerState* mShadowTexSampler;
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
    ID3D11Buffer* mShadowStructedBuffer;
    ID3D11ShaderResourceView* mShadowStructedBufferSrv;
    ID3D11ShaderResourceView* mSsaoSrv;
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

class RSPass_Normal :public RSPass_Base
{
public:
    RSPass_Normal(std::string& _name, PASS_TYPE _type,
        class RSRoot_DX11* _root);
    RSPass_Normal(const RSPass_Normal& _source);
    virtual ~RSPass_Normal();

public:
    virtual RSPass_Normal* ClonePass() override;

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
    DRAWCALL_TYPE mDrawCallType;
    RSDrawCallsPipe* mDrawCallPipe;
    ID3D11Buffer* mViewProjStructedBuffer;
    ID3D11ShaderResourceView* mViewProjStructedBufferSrv;
    ID3D11Buffer* mInstanceStructedBuffer;
    ID3D11ShaderResourceView* mInstanceStructedBufferSrv;
};

struct SsaoInfo
{
    DirectX::XMFLOAT4X4 mProj;
    DirectX::XMFLOAT4X4 mInvProj;
    DirectX::XMFLOAT4X4 mTexProj;
    DirectX::XMFLOAT4 mOffsetVec[14];
    float mOcclusionRadius;
    float mOcclusionFadeStart;
    float mOcclusionFadeEnd;
    float mSurfaceEpsilon;
};

class RSPass_Ssao :public RSPass_Base
{
public:
    RSPass_Ssao(std::string& _name, PASS_TYPE _type,
        class RSRoot_DX11* _root);
    RSPass_Ssao(const RSPass_Ssao& _source);
    virtual ~RSPass_Ssao();

public:
    virtual RSPass_Ssao* ClonePass() override;

    virtual bool InitPass();

    virtual void ReleasePass();

    virtual void ExecuatePass();

private:
    bool CreateShaders();
    bool CreateBuffers();
    bool CreateTextures();
    bool CreateViews();
    bool CreateSamplers();

private:
    ID3D11VertexShader* mVertexShader;
    ID3D11PixelShader* mPixelShader;
    ID3D11RenderTargetView* mRenderTargetView;
    ID3D11SamplerState* mSamplePointClamp;
    ID3D11SamplerState* mSampleLinearClamp;
    ID3D11SamplerState* mSampleDepthMap;
    ID3D11SamplerState* mSampleLinearWrap;
    ID3D11Buffer* mSsaoInfoStructedBuffer;
    ID3D11ShaderResourceView* mSsaoInfoStructedBufferSrv;
    ID3D11ShaderResourceView* mNormalMapSrv;
    ID3D11ShaderResourceView* mDepthMapSrv;
    ID3D11ShaderResourceView* mRandomMapSrv;
    DirectX::XMFLOAT4 mOffsetVec[14];
    ID3D11Buffer* mVertexBuffer;
    ID3D11Buffer* mIndexBuffer;
};

class RSPass_KBBlur :public RSPass_Base
{
public:
    RSPass_KBBlur(std::string& _name, PASS_TYPE _type,
        class RSRoot_DX11* _root);
    RSPass_KBBlur(const RSPass_KBBlur& _source);
    virtual ~RSPass_KBBlur();

public:
    virtual RSPass_KBBlur* ClonePass() override;

    virtual bool InitPass();

    virtual void ReleasePass();

    virtual void ExecuatePass();

private:
    bool CreateShaders();
    bool CreateViews();

private:
    ID3D11ComputeShader* mHoriBlurShader;
    ID3D11ComputeShader* mVertBlurShader;
    ID3D11UnorderedAccessView* mSsaoTexUav;
    ID3D11ShaderResourceView* mNormalMapSrv;
    ID3D11ShaderResourceView* mDepthMapSrv;
};

struct SkyShpereInfo
{
    DirectX::XMFLOAT4X4 mWorldMat = {};
    DirectX::XMFLOAT4X4 mViewMat = {};
    DirectX::XMFLOAT4X4 mProjMat = {};
    DirectX::XMFLOAT3 mEyePosition = {};
    float mPad = 0.f;
};

class RSPass_SkyShpere :public RSPass_Base
{
public:
    RSPass_SkyShpere(std::string& _name, PASS_TYPE _type,
        class RSRoot_DX11* _root);
    RSPass_SkyShpere(const RSPass_SkyShpere& _source);
    virtual ~RSPass_SkyShpere();

public:
    virtual RSPass_SkyShpere* ClonePass() override;

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
    ID3D11DepthStencilState* mDepthStencilState;
    ID3D11SamplerState* mLinearWrapSampler;
    ID3D11RenderTargetView* mRenderTargerView;
    ID3D11DepthStencilView* mDepthStencilView;
    //ID3D11ShaderResourceView* mSkyShpereSrv;
    ID3D11Buffer* mSkyShpereInfoStructedBuffer;
    ID3D11ShaderResourceView* mSkyShpereInfoStructedBufferSrv;
    RS_SUBMESH_DATA mSkySphereMesh;
};

class RSPass_Font :public RSPass_Base
{
public:
    RSPass_Font(std::string& _name, PASS_TYPE _type,
        class RSRoot_DX11* _root);
    RSPass_Font(const RSPass_Font& _source);
    virtual ~RSPass_Font();

public:
    virtual RSPass_Font* ClonePass() override;

    virtual bool InitPass();

    virtual void ReleasePass();

    virtual void ExecuatePass();

private:
    bool CreateFonts();

private:
    DirectX::SpriteBatch* mSpriteBatch;
    DirectX::SpriteFont* mSpriteFont;
    ID3D11RenderTargetView* mRenderTargetView;
    ID3D11DepthStencilView* mDepthStencilView;
};
