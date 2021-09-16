#include "TempPipeline_Diffuse.h"
#include "RSRoot_DX11.h"
#include "RSPipeline.h"
#include "RSTopic.h"
#include "RSShaderCompile.h"
#include "RSDevices.h"

#define RS_RELEASE(p) { if (p) { (p)->Release(); (p)=nullptr; } }
static RSRoot_DX11* g_Root = nullptr;
static RSPipeline* g_TempPipeline = nullptr;

void PassRootToTempPipeline(class RSRoot_DX11* _root)
{
    g_Root = _root;
}

bool CreateTempPipeline()
{
    std::string name = "test-diffuse";
    RSPass_Diffuse* pass = new RSPass_Diffuse(
        name, PASS_TYPE::RENDER);

    pass->SetExecuateOrder(1);

    name = "test-topic";
    RSTopic* topic = new RSTopic(name);
    topic->StartTopicAssembly();
    topic->InsertPass(pass);
    topic->SetExecuateOrder(1);
    topic->FinishTopicAssembly();

    name = "test-pipeline";
    g_TempPipeline = new RSPipeline(name);
    g_TempPipeline->StartPipelineAssembly();
    g_TempPipeline->InsertTopic(topic);
    g_TempPipeline->FinishPipelineAssembly();

    if (!g_TempPipeline->InitAllTopics()) { return false; }

    return true;
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
    mDrawCallPipe(nullptr), mWVPBuffer(nullptr),
    mCPUBuffer({})
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
    mWVPBuffer(_source.mWVPBuffer),
    mCPUBuffer(_source.mCPUBuffer)
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
    ID3DBlob* shaderBlob = nullptr;
    HRESULT hr = S_OK;

    hr = Tool::CompileShaderFromFile(
        L".\\Shaders\\diffuse_vertex.hlsl",
        "main", "vs_5_0", &shaderBlob);
    if (FAILED(hr)) { return false; }

    hr = g_Root->Devices()->GetDevice()->CreateVertexShader(
        shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(),
        nullptr, &mVertexShader);
    shaderBlob->Release();
    shaderBlob = nullptr;
    if (FAILED(hr)) { return false; }

    hr = Tool::CompileShaderFromFile(
        L".\\Shaders\\diffuse_pixel.hlsl",
        "main", "ps_5_0", &shaderBlob);
    if (FAILED(hr)) { return false; }

    hr = g_Root->Devices()->GetDevice()->CreatePixelShader(
        shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(),
        nullptr, &mPixelShader);
    shaderBlob->Release();
    shaderBlob = nullptr;
    if (FAILED(hr)) { return false; }

    return true;
}

bool RSPass_Diffuse::CreateStates()
{
    return true;
}

bool RSPass_Diffuse::CreateViews()
{
    mRenderTargetView = g_Root->Devices()->GetSwapChainRtv();

    HRESULT hr = S_OK;
    ID3D11Texture2D* depthTex = nullptr;
    D3D11_TEXTURE2D_DESC texDepSte = {};
    texDepSte.Width = 1280;
    texDepSte.Height = 720;
    texDepSte.MipLevels = 1;
    texDepSte.ArraySize = 1;
    texDepSte.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    texDepSte.SampleDesc.Count = 1;
    texDepSte.SampleDesc.Quality = 0;
    texDepSte.Usage = D3D11_USAGE_DEFAULT;
    texDepSte.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    texDepSte.CPUAccessFlags = 0;
    texDepSte.MiscFlags = 0;
    hr = g_Root->Devices()->GetDevice()->CreateTexture2D(
        &texDepSte, nullptr, &depthTex);
    if (FAILED(hr)) { return false; }

    D3D11_DEPTH_STENCIL_VIEW_DESC desDSV = {};
    desDSV.Format = texDepSte.Format;
    desDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    desDSV.Texture2D.MipSlice = 0;
    hr = g_Root->Devices()->GetDevice()->CreateDepthStencilView(
        depthTex, &desDSV, &mDepthStencilView);
    depthTex->Release();
    if (FAILED(hr)) { return false; }

    return true;
}

bool RSPass_Diffuse::CreateSamplers()
{
    HRESULT hr = S_OK;
    D3D11_SAMPLER_DESC sampDesc = {};
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    hr = g_Root->Devices()->GetDevice()->CreateSamplerState(
        &sampDesc, &mSampler);
    if (FAILED(hr)) { return false; }

    return true;
}

bool RSPass_Diffuse::CreateBuffers()
{
    HRESULT hr = S_OK;
    D3D11_BUFFER_DESC bdc = {};
    bdc.Usage = D3D11_USAGE_DEFAULT;
    bdc.ByteWidth = sizeof(TEMP_WVPBuffer);
    bdc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bdc.CPUAccessFlags = 0;
    hr = g_Root->Devices()->GetDevice()->CreateBuffer(
        &bdc, nullptr, &mWVPBuffer);
    if (FAILED(hr)) { return false; }

    return true;
}
