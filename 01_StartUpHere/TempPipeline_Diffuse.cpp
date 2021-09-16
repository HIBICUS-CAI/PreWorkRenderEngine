#include "TempPipeline_Diffuse.h"
#include "RSRoot_DX11.h"
#include "RSPipeline.h"

#define RS_RELEASE(p) { if (p) { (p)->Release(); (p)=nullptr; } }
static RSRoot_DX11* g_Root = nullptr;
static RSPipeline* g_TempPipeline = nullptr;

void PassRootToTempPipeline(class RSRoot_DX11* _root)
{
    g_Root = _root;
}

bool CreateTempPipeline()
{
    return false;
}

void ExecuateTempPipeline()
{
    g_TempPipeline->ExecuatePipeline();
}

void ReleaseTempPipeline()
{
    g_TempPipeline->ReleasePipeline();
}

RSPass_Diffuse::RSPass_Diffuse(
    std::string& _name, PASS_TYPE _type) :
    RSPass_Base(_name, _type),
    mVertexShader(nullptr), mPixelShader(nullptr),
    //mRasterizerState(nullptr), mDepthStencilState(nullptr),
    mRenderTargetView(nullptr), mDepthStencilView(nullptr),
    mSampler(nullptr), mDrawCallType(DRAWCALL_TYPE::OPACITY),
    mDrawCallPipe(nullptr), mWVPBuffer(nullptr)
{

}

RSPass_Diffuse::RSPass_Diffuse(
    const RSPass_Diffuse& _source) :
    RSPass_Base(_source),
    mVertexShader(_source.mVertexShader),
    mPixelShader(_source.mPixelShader),
    //mRasterizerState(_source.mRasterizerState),
    //mDepthStencilState(_source.mDepthStencilState),
    mRenderTargetView(_source.mRenderTargetView),
    mDepthStencilView(_source.mDepthStencilView),
    //mSamplers(_source.mSamplers),
    mSampler(_source.mSampler),
    mDrawCallType(_source.mDrawCallType),
    mDrawCallPipe(_source.mDrawCallPipe),
    mWVPBuffer(_source.mWVPBuffer)
{

}

RSPass_Diffuse::~RSPass_Diffuse()
{

}

RSPass_Diffuse* RSPass_Diffuse::ClonePass()
{
    return new RSPass_Diffuse(*this);
}

bool RSPass_Diffuse::InitPass()
{
    if (!CreateShaders()) { return false; }
    if (!CreateStates()) { return false; }
    if (!CreateViews()) { return false; }
    if (!CreateSamplers()) { return false; }
    if (!CreateBuffers()) { return false; }

    return true;
}

void RSPass_Diffuse::ReleasePass()
{
    RS_RELEASE(mVertexShader);
    RS_RELEASE(mPixelShader);
    RS_RELEASE(mSampler);
    RS_RELEASE(mDepthStencilView);
    RS_RELEASE(mWVPBuffer);
}

void RSPass_Diffuse::ExecuatePass()
{

}

bool RSPass_Diffuse::CreateShaders()
{
    return false;
}

bool RSPass_Diffuse::CreateStates()
{
    return false;
}

bool RSPass_Diffuse::CreateViews()
{
    return false;
}

bool RSPass_Diffuse::CreateSamplers()
{
    return false;
}

bool RSPass_Diffuse::CreateBuffers()
{
    return false;
}
