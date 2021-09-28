#include "TempPipeline_Light.h"
#include "RSRoot_DX11.h"
#include "RSPipeline.h"
#include "RSTopic.h"
#include "RSShaderCompile.h"
#include "RSDevices.h"
#include "RSDrawCallsPool.h"
#include "RSStaticResources.h"
#include "RSPipelinesManager.h"
#include <DirectXColors.h>
#include "TempMesh.h"

#define RS_RELEASE(p) { if (p) { (p)->Release(); (p)=nullptr; } }
static RSRoot_DX11* g_Root = nullptr;
static RSPipeline* g_TempPipeline = nullptr;

void PassRootToTempLightPipeline(RSRoot_DX11* _root)
{
    g_Root = _root;
}

bool CreateTempLightPipeline()
{
    return false;
}

RSPass_Light::RSPass_Light(
    std::string& _name, PASS_TYPE _type, RSRoot_DX11* _root) :
    RSPass_Base(_name, _type, _root),
    mVertexShader(nullptr), mPixelShader(nullptr),
    mRasterizerState(nullptr), mDepthStencilView(nullptr),
    mMeshTexSampler(nullptr),
    mDrawCallType(DRAWCALL_TYPE::OPACITY),
    mDrawCallPipe(nullptr),
    mViewProjStructedBuffer(nullptr),
    mViewProjStructedBufferSrv(nullptr),
    mInstanceStructedBuffer(nullptr),
    mInstanceStructedBufferSrv(nullptr),
    mLightStructedBuffer(nullptr),
    mLightStructedBufferSrv(nullptr),
    mAmbientStructedBuffer(nullptr),
    mAmbientStructedBufferSrv(nullptr)
{

}

RSPass_Light::RSPass_Light(const RSPass_Light& _source) :
    RSPass_Base(_source),
    mVertexShader(_source.mVertexShader),
    mPixelShader(_source.mPixelShader),
    mRasterizerState(_source.mRasterizerState),
    mDepthStencilView(_source.mDepthStencilView),
    mMeshTexSampler(_source.mMeshTexSampler),
    mDrawCallType(_source.mDrawCallType),
    mDrawCallPipe(_source.mDrawCallPipe),
    mViewProjStructedBuffer(_source.mViewProjStructedBuffer),
    mViewProjStructedBufferSrv(_source.mViewProjStructedBufferSrv),
    mInstanceStructedBuffer(_source.mInstanceStructedBuffer),
    mInstanceStructedBufferSrv(_source.mInstanceStructedBufferSrv),
    mLightStructedBuffer(_source.mLightStructedBuffer),
    mLightStructedBufferSrv(_source.mLightStructedBufferSrv),
    mAmbientStructedBuffer(_source.mAmbientStructedBuffer),
    mAmbientStructedBufferSrv(_source.mAmbientStructedBufferSrv)
{

}

RSPass_Light::~RSPass_Light()
{

}

RSPass_Light* RSPass_Light::ClonePass()
{
    return new RSPass_Light(*this);
}

bool RSPass_Light::InitPass()
{
    if (!CreateShaders()) { return false; }
    if (!CreateStates()) { return false; }
    if (!CreateBuffers()) { return false; }
    if (!CreateViews()) { return false; }
    if (!CreateSamplers()) { return false; }

    mDrawCallType = DRAWCALL_TYPE::OPACITY;
    mDrawCallPipe = g_Root->DrawCallsPool()->
        GetDrawCallsPipe(mDrawCallType);

    return true;
}

void RSPass_Light::ReleasePass()
{
    RS_RELEASE(mVertexShader);
    RS_RELEASE(mPixelShader);
    RS_RELEASE(mRasterizerState);
    RS_RELEASE(mMeshTexSampler);
    RS_RELEASE(mViewProjStructedBufferSrv);
    RS_RELEASE(mViewProjStructedBuffer);
    RS_RELEASE(mInstanceStructedBufferSrv);
    RS_RELEASE(mInstanceStructedBuffer);
    RS_RELEASE(mLightStructedBufferSrv);
    RS_RELEASE(mLightStructedBuffer);
    RS_RELEASE(mAmbientStructedBufferSrv);
    RS_RELEASE(mAmbientStructedBuffer);

    std::string name = "";
    g_Root->TexturesManager()->DeleteDataTex(name);
}

void RSPass_Light::ExecuatePass()
{

}

bool RSPass_Light::CreateShaders()
{
    return true;
}

bool RSPass_Light::CreateStates()
{
    return true;
}

bool RSPass_Light::CreateBuffers()
{
    return true;
}

bool RSPass_Light::CreateViews()
{
    return true;
}

bool RSPass_Light::CreateSamplers()
{
    return true;
}
