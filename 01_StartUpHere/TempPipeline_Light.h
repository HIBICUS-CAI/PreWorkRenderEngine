#pragma once

#include <string>
#include <vector>
#include <array>
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
    UINT mShadowLightNum = 0;
    INT mShadowLightIndex[4] = { -1,-1,-1,-1 };
};

struct ShadowInfo
{
    DirectX::XMFLOAT4X4 mShadowViewMat = {};
    DirectX::XMFLOAT4X4 mShadowProjMat = {};
    DirectX::XMFLOAT4X4 mSSAOMat = {};
};

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
    ID3D11DepthStencilState* mDepthStencilState;
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
    RS_CAM_INFO* mRSCameraInfo;
};

class RSPass_Defered :public RSPass_Base
{
public:
    RSPass_Defered(std::string& _name, PASS_TYPE _type,
        class RSRoot_DX11* _root);
    RSPass_Defered(const RSPass_Defered& _source);
    virtual ~RSPass_Defered();

public:
    virtual RSPass_Defered* ClonePass() override;

    virtual bool InitPass();

    virtual void ReleasePass();

    virtual void ExecuatePass();

private:
    bool CreateShaders();
    bool CreateBuffers();
    bool CreateViews();
    bool CreateSamplers();

private:
    ID3D11VertexShader* mVertexShader;
    ID3D11PixelShader* mPixelShader;
    ID3D11RenderTargetView* mRenderTargetView;
    ID3D11SamplerState* mLinearWrapSampler;
    ID3D11SamplerState* mPointClampSampler;
    ID3D11SamplerState* mShadowTexSampler;
    ID3D11Buffer* mLightInfoStructedBuffer;
    ID3D11ShaderResourceView* mLightInfoStructedBufferSrv;
    ID3D11Buffer* mLightStructedBuffer;
    ID3D11ShaderResourceView* mLightStructedBufferSrv;
    ID3D11Buffer* mAmbientStructedBuffer;
    ID3D11ShaderResourceView* mAmbientStructedBufferSrv;
    ID3D11Buffer* mShadowStructedBuffer;
    ID3D11ShaderResourceView* mShadowStructedBufferSrv;
    ID3D11ShaderResourceView* mWorldPosSrv;
    ID3D11ShaderResourceView* mNormalSrv;
    ID3D11ShaderResourceView* mDiffuseSrv;
    ID3D11ShaderResourceView* mDiffuseAlbedoSrv;
    ID3D11ShaderResourceView* mFresenlShineseSrv;
    ID3D11ShaderResourceView* mSsaoSrv;
    ID3D11ShaderResourceView* mShadowDepthSrv;
    ID3D11Buffer* mVertexBuffer;
    ID3D11Buffer* mIndexBuffer;
    RS_CAM_INFO* mRSCameraInfo;
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
    std::array<ID3D11DepthStencilView*, MAX_SHADOW_SIZE> mDepthStencilView;
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
    DirectX::XMFLOAT4X4 mView;
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
    RS_CAM_INFO* mRSCameraInfo;
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
    RS_CAM_INFO* mRSCameraInfo;
};

struct OnlyProj
{
    DirectX::XMFLOAT4X4 mProjMat = {};
};

class RSPass_Sprite :public RSPass_Base
{
public:
    RSPass_Sprite(std::string& _name, PASS_TYPE _type,
        class RSRoot_DX11* _root);
    RSPass_Sprite(const RSPass_Sprite& _source);
    virtual ~RSPass_Sprite();

public:
    virtual RSPass_Sprite* ClonePass() override;

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
    ID3D11DepthStencilState* mDepthStencilState;
    ID3D11BlendState* mBlendState;
    ID3D11RenderTargetView* mRenderTargetView;
    DRAWCALL_TYPE mDrawCallType;
    RSDrawCallsPipe* mDrawCallPipe;
    ID3D11Buffer* mProjStructedBuffer;
    ID3D11ShaderResourceView* mProjStructedBufferSrv;
    ID3D11Buffer* mInstanceStructedBuffer;
    ID3D11ShaderResourceView* mInstanceStructedBufferSrv;
    ID3D11SamplerState* mLinearSampler;
    RS_CAM_INFO* mRSCameraInfo;
};

class RSPass_MRT :public RSPass_Base
{
public:
    RSPass_MRT(std::string& _name, PASS_TYPE _type,
        class RSRoot_DX11* _root);
    RSPass_MRT(const RSPass_MRT& _source);
    virtual ~RSPass_MRT();

public:
    virtual RSPass_MRT* ClonePass() override;

    virtual bool InitPass();

    virtual void ReleasePass();

    virtual void ExecuatePass();

private:
    bool CreateShaders();
    bool CreateBuffers();
    bool CreateViews();
    bool CreateSamplers();

private:
    ID3D11VertexShader* mVertexShader;
    ID3D11PixelShader* mPixelShader;
    ID3D11PixelShader* mNDPixelShader;
    DRAWCALL_TYPE mDrawCallType;
    RSDrawCallsPipe* mDrawCallPipe;
    ID3D11Buffer* mViewProjStructedBuffer;
    ID3D11ShaderResourceView* mViewProjStructedBufferSrv;
    ID3D11Buffer* mInstanceStructedBuffer;
    ID3D11ShaderResourceView* mInstanceStructedBufferSrv;
    ID3D11SamplerState* mLinearSampler;
    ID3D11RenderTargetView* mDiffuseRtv;
    ID3D11RenderTargetView* mNormalRtv;
    ID3D11RenderTargetView* mWorldPosRtv;
    ID3D11RenderTargetView* mDiffAlbeRtv;
    ID3D11RenderTargetView* mFresShinRtv;
    ID3D11DepthStencilView* mDepthDsv;
    RS_CAM_INFO* mRSCameraInfo;
};

class RSPass_Bloom :public RSPass_Base
{
public:
    RSPass_Bloom(std::string& _name, PASS_TYPE _type,
        class RSRoot_DX11* _root);
    RSPass_Bloom(const RSPass_Bloom& _source);
    virtual ~RSPass_Bloom();

public:
    virtual RSPass_Bloom* ClonePass() override;

    virtual bool InitPass();

    virtual void ReleasePass();

    virtual void ExecuatePass();

private:
    bool CreateShaders();
    bool CreateBuffers();
    bool CreateViews();

private:
    ID3D11VertexShader* mVertexShader;
    ID3D11PixelShader* mPixelShader;
    DRAWCALL_TYPE mDrawCallType;
    RSDrawCallsPipe* mDrawCallPipe;
    ID3D11Buffer* mViewProjStructedBuffer;
    ID3D11ShaderResourceView* mViewProjStructedBufferSrv;
    ID3D11Buffer* mInstanceStructedBuffer;
    ID3D11ShaderResourceView* mInstanceStructedBufferSrv;
    ID3D11RenderTargetView* mRtv;
    ID3D11DepthStencilView* mDepthDsv;
    RS_CAM_INFO* mRSCameraInfo;
};

class RSPass_BloomOn :public RSPass_Base
{
public:
    RSPass_BloomOn(std::string& _name, PASS_TYPE _type,
        class RSRoot_DX11* _root);
    RSPass_BloomOn(const RSPass_BloomOn& _source);
    virtual ~RSPass_BloomOn();

public:
    virtual RSPass_BloomOn* ClonePass() override;

    virtual bool InitPass();

    virtual void ReleasePass();

    virtual void ExecuatePass();

private:
    bool CreateShaders();
    bool CreateBuffers();
    bool CreateViews();
    bool CreateStates();
    bool CreateSamplers();

private:
    ID3D11VertexShader* mVertexShader;
    ID3D11PixelShader* mPixelShader;
    ID3D11RenderTargetView* mRtv;
    ID3D11ShaderResourceView* mBloomTexSrv;
    ID3D11BlendState* mBlendState;
    ID3D11DepthStencilState* mDepthState;
    ID3D11SamplerState* mSampler;
    ID3D11Buffer* mVertexBuffer;
    ID3D11Buffer* mIndexBuffer;
};

class RSPass_Blur :public RSPass_Base
{
public:
    RSPass_Blur(std::string& _name, PASS_TYPE _type,
        class RSRoot_DX11* _root);
    RSPass_Blur(const RSPass_Blur& _source);
    virtual ~RSPass_Blur();

public:
    virtual RSPass_Blur* ClonePass() override;

    virtual bool InitPass();

    virtual void ReleasePass();

    virtual void ExecuatePass();

private:
    bool CreateShaders();
    bool CreateViews();

private:
    ID3D11ComputeShader* mHoriBlurShader;
    ID3D11ComputeShader* mVertBlurShader;
    ID3D11UnorderedAccessView* mLightTexUav;
};
