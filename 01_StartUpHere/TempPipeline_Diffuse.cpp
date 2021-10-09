#include "TempPipeline_Diffuse.h"
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
#include "TempRenderConfig.h"

#define IN_ONE_TOPIC

#define RS_RELEASE(p) { if (p) { (p)->Release(); (p)=nullptr; } }
static RSRoot_DX11* g_Root = nullptr;
static RSPipeline* g_TempPipeline = nullptr;
static D3D11_VIEWPORT g_ViewPort = {};

bool CreateTempWireFramePipeline()
{
    g_Root = GetRSRoot_DX11_Singleton();

    std::string name = "test-diffuse";
    RSPass_Diffuse* pass = new RSPass_Diffuse(
        name, PASS_TYPE::RENDER, g_Root);

    pass->SetExecuateOrder(1);

    std::string name1 = "test-fromtex";
    RSPass_FromTex* pass1 = new RSPass_FromTex(
        name1, PASS_TYPE::RENDER, g_Root);
    pass1->SetExecuateOrder(2);

#ifdef IN_ONE_TOPIC
    name = "test-topic";
    RSTopic* topic = new RSTopic(name);
    topic->StartTopicAssembly();
    topic->InsertPass(pass1);
    topic->InsertPass(pass);
    topic->SetExecuateOrder(1);
    topic->FinishTopicAssembly();

    name = "test-pipeline";
    g_TempPipeline = new RSPipeline(name);
    g_TempPipeline->StartPipelineAssembly();
    g_TempPipeline->InsertTopic(topic);
    g_TempPipeline->FinishPipelineAssembly();
#else
    name = "test-topic1";
    RSTopic* topic1 = new RSTopic(name);
    topic1->StartTopicAssembly();
    topic1->InsertPass(pass);
    topic1->SetExecuateOrder(1);
    topic1->FinishTopicAssembly();

    name = "test-topic2";
    RSTopic* topic2 = new RSTopic(name);
    topic2->StartTopicAssembly();
    topic2->InsertPass(pass1);
    topic2->SetExecuateOrder(2);
    topic2->FinishTopicAssembly();

    name = "test-pipeline";
    g_TempPipeline = new RSPipeline(name);
    g_TempPipeline->StartPipelineAssembly();
    g_TempPipeline->InsertTopic(topic2);
    g_TempPipeline->InsertTopic(topic1);
    g_TempPipeline->FinishPipelineAssembly();
#endif // IN_ONE_TOPIC

    if (!g_TempPipeline->InitAllTopics(g_Root->Devices(),
        GetRenderConfig().mForceSingleThreadEnable))
    {
        return false;
    }

    name = g_TempPipeline->GetPipelineName();
    g_Root->PipelinesManager()->AddPipeline(
        name, g_TempPipeline);
    g_Root->PipelinesManager()->SetPipeline(name);
    g_Root->PipelinesManager()->ProcessNextPipeline();

    g_ViewPort.Width = 1280.f;
    g_ViewPort.Height = 720.f;
    g_ViewPort.MinDepth = 0.f;
    g_ViewPort.MaxDepth = 1.f;
    g_ViewPort.TopLeftX = 0.f;
    g_ViewPort.TopLeftY = 0.f;

    return true;
}

RSPass_Diffuse::RSPass_Diffuse(
    std::string& _name, PASS_TYPE _type, RSRoot_DX11* _root) :
    RSPass_Base(_name, _type, _root),
    mVertexShader(nullptr), mPixelShader(nullptr),
    mRasterizerState(nullptr), /*mDepthStencilState(nullptr),*/
    mRenderTargetView(nullptr), mDepthStencilView(nullptr),
    mSampler(nullptr), mDrawCallType(DRAWCALL_TYPE::OPACITY),
    mDrawCallPipe(nullptr), mWVPBuffer(nullptr),
    mCPUBuffer({}), mStructedBuffer(nullptr),
    mStructedBufferSrv(nullptr), mRSCameraInfo(nullptr)
{

}

RSPass_Diffuse::RSPass_Diffuse(
    const RSPass_Diffuse& _source) :
    RSPass_Base(_source),
    mVertexShader(_source.mVertexShader),
    mPixelShader(_source.mPixelShader),
    mRasterizerState(_source.mRasterizerState),
    //mDepthStencilState(_source.mDepthStencilState),
    mRenderTargetView(_source.mRenderTargetView),
    mDepthStencilView(_source.mDepthStencilView),
    //mSamplers(_source.mSamplers),
    mSampler(_source.mSampler),
    mDrawCallType(_source.mDrawCallType),
    mDrawCallPipe(_source.mDrawCallPipe),
    mWVPBuffer(_source.mWVPBuffer),
    mCPUBuffer(_source.mCPUBuffer),
    mStructedBuffer(_source.mStructedBuffer),
    mStructedBufferSrv(_source.mStructedBufferSrv),
    mRSCameraInfo(_source.mRSCameraInfo)
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
    if (!CreateBuffers()) { return false; }
    if (!CreateViews()) { return false; }
    if (!CreateSamplers()) { return false; }

    mDrawCallType = DRAWCALL_TYPE::OPACITY;
    mDrawCallPipe = g_Root->DrawCallsPool()->
        GetDrawCallsPipe(mDrawCallType);

    std::string name = "temp-cam";
    mRSCameraInfo = g_Root->CamerasContainer()->
        GetRSCameraInfo(name);

    return true;
}

void RSPass_Diffuse::ReleasePass()
{
    RS_RELEASE(mVertexShader);
    RS_RELEASE(mPixelShader);
    RS_RELEASE(mSampler);
    RS_RELEASE(mWVPBuffer);
    RS_RELEASE(mStructedBuffer);
    RS_RELEASE(mRasterizerState);

    std::string name = "temp-tex-depth";
    g_Root->TexturesManager()->DeleteDataTex(name);
    name = "temp-tex";
    g_Root->TexturesManager()->DeleteDataTex(name);
}

void RSPass_Diffuse::ExecuatePass()
{
    STContext()->OMSetRenderTargets(1,
        &mRenderTargetView, mDepthStencilView);
    STContext()->RSSetViewports(1, &g_ViewPort);
    STContext()->ClearRenderTargetView(
        mRenderTargetView, DirectX::Colors::DarkGreen);
    STContext()->ClearDepthStencilView(
        mDepthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);
    STContext()->VSSetShader(mVertexShader, nullptr, 0);
    STContext()->PSSetShader(mPixelShader, nullptr, 0);
    STContext()->PSSetSamplers(0, 1, &mSampler);
    STContext()->RSSetState(mRasterizerState);

    DirectX::XMMATRIX mat = {};
    DirectX::XMFLOAT4X4 flt44 = {};
    UINT stride = sizeof(VERTEX_INFO);
    UINT offset = 0;
    for (auto& call : mDrawCallPipe->mDatas)
    {
        auto vecPtr = call.mInstanceData.mDataPtr;
        auto size = vecPtr->size();
        D3D11_MAPPED_SUBRESOURCE msr = {};
        STContext()->Map(mStructedBuffer, 0,
            D3D11_MAP_WRITE_DISCARD, 0, &msr);
        RS_INSTANCE_DATA* ins_data = (RS_INSTANCE_DATA*)msr.pData;
        for (size_t i = 0; i < size; i++)
        {
            mat = DirectX::XMLoadFloat4x4(
                &(*vecPtr)[i].mWorldMat);
            mat = DirectX::XMMatrixTranspose(mat);
            DirectX::XMStoreFloat4x4(&ins_data[i].mWorldMat, mat);
            ins_data[i].mMaterialData =
                (*vecPtr)[i].mMaterialData;
            ins_data[i].mCustomizedData1 =
                (*vecPtr)[i].mCustomizedData1;
            ins_data[i].mCustomizedData2 =
                (*vecPtr)[i].mCustomizedData2;
        }
        STContext()->Unmap(mStructedBuffer, 0);

        mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mViewMat);
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&flt44, mat);
        mCPUBuffer.mView = flt44;
        mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mProjMat);
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&flt44, mat);
        mCPUBuffer.mProjection = flt44;

        STContext()->UpdateSubresource(
            mWVPBuffer, 0, nullptr, &mCPUBuffer, 0, 0);
        STContext()->IASetInputLayout(
            call.mMeshData.mLayout);
        STContext()->IASetPrimitiveTopology(
            call.mMeshData.mTopologyType);
        STContext()->IASetVertexBuffers(
            0, 1, &call.mMeshData.mVertexBuffer,
            &stride, &offset);
        STContext()->IASetIndexBuffer(
            call.mMeshData.mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
        STContext()->VSSetConstantBuffers(0, 1, &mWVPBuffer);
        STContext()->VSSetShaderResources(
            0, 1, &mStructedBufferSrv);
        STContext()->PSSetShaderResources(
            0, 1, &call.mTextureDatas[0].mSrv);

        STContext()->DrawIndexedInstanced(
            call.mMeshData.mIndexCount,
            (UINT)call.mInstanceData.mDataPtr->size(), 0, 0, 0);
    }

    ID3D11RenderTargetView* null = nullptr;
    STContext()->OMSetRenderTargets(1, &null, nullptr);
}

bool RSPass_Diffuse::CreateShaders()
{
    ID3DBlob* shaderBlob = nullptr;
    HRESULT hr = S_OK;

    hr = Tool::CompileShaderFromFile(
        L".\\Shaders\\diffuse_vertex.hlsl",
        "main", "vs_5_0", &shaderBlob);
    if (FAILED(hr)) { return false; }

    hr = Device()->CreateVertexShader(
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

    hr = Device()->CreatePixelShader(
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
    HRESULT hr = S_OK;
    D3D11_RASTERIZER_DESC rasDesc = {};

    rasDesc.FillMode = D3D11_FILL_WIREFRAME;
    rasDesc.CullMode = D3D11_CULL_NONE;
    rasDesc.FrontCounterClockwise = FALSE;
    rasDesc.DepthBias = 0;
    rasDesc.SlopeScaledDepthBias = 0.f;
    rasDesc.DepthBiasClamp = 0.f;
    rasDesc.DepthClipEnable = TRUE;
    rasDesc.ScissorEnable = FALSE;
    rasDesc.MultisampleEnable = FALSE;
    rasDesc.AntialiasedLineEnable = FALSE;

    hr = Device()->CreateRasterizerState(
        &rasDesc, &mRasterizerState);
    if (FAILED(hr)) { return false; }

    return true;
}

bool RSPass_Diffuse::CreateViews()
{
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
    hr = Device()->CreateTexture2D(
        &texDepSte, nullptr, &depthTex);
    if (FAILED(hr)) { return false; }

    D3D11_DEPTH_STENCIL_VIEW_DESC desDSV = {};
    desDSV.Format = texDepSte.Format;
    desDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    desDSV.Texture2D.MipSlice = 0;
    hr = Device()->CreateDepthStencilView(
        depthTex, &desDSV, &mDepthStencilView);
    if (FAILED(hr)) { return false; }

    DATA_TEXTURE_INFO dti = {};
    std::string name = "temp-tex-depth";
    dti.mTexture = depthTex;
    dti.mDsv = mDepthStencilView;
    g_Root->TexturesManager()->AddDataTexture(name, dti);

    name = "temp-tex";
    ID3D11Texture2D* tempTex = nullptr;
    D3D11_TEXTURE2D_DESC tempDesc = {};
    tempDesc.Width = 1280;
    tempDesc.Height = 720;
    tempDesc.MipLevels = 1;
    tempDesc.ArraySize = 1;
    tempDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    tempDesc.SampleDesc.Count = 1;
    tempDesc.SampleDesc.Quality = 0;
    tempDesc.Usage = D3D11_USAGE_DEFAULT;
    tempDesc.BindFlags =
        D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    tempDesc.CPUAccessFlags = 0;
    tempDesc.MiscFlags = 0;
    hr = Device()->CreateTexture2D(
        &tempDesc, nullptr, &tempTex);
    if (FAILED(hr)) { return false; }

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvDesc.Texture2D.MipSlice = 0;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    hr = Device()->CreateRenderTargetView(tempTex, &rtvDesc,
        &mRenderTargetView);
    if (FAILED(hr)) { return false; }

    ID3D11ShaderResourceView* srv = nullptr;
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;
    hr = Device()->CreateShaderResourceView(tempTex, &srvDesc,
        &srv);
    if (FAILED(hr)) { return false; }

    dti = {};
    dti.mTexture = tempTex;
    dti.mRtv = mRenderTargetView;
    dti.mSrv = srv;
    g_Root->TexturesManager()->AddDataTexture(name, dti);

    D3D11_SHADER_RESOURCE_VIEW_DESC desSRV = {};
    ZeroMemory(&desSRV, sizeof(desSRV));
    desSRV.Format = DXGI_FORMAT_UNKNOWN;
    desSRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    desSRV.Buffer.ElementWidth = 256;
    hr = Device()->CreateShaderResourceView(mStructedBuffer,
        &desSRV, &mStructedBufferSrv);
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

    hr = Device()->CreateSamplerState(
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
    hr = Device()->CreateBuffer(
        &bdc, nullptr, &mWVPBuffer);
    if (FAILED(hr)) { return false; }

    ZeroMemory(&bdc, sizeof(bdc));
    bdc.Usage = D3D11_USAGE_DYNAMIC;
    bdc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bdc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    bdc.ByteWidth = 256 * sizeof(RS_INSTANCE_DATA);
    bdc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bdc.StructureByteStride = sizeof(RS_INSTANCE_DATA);
    hr = Device()->CreateBuffer(
        &bdc, nullptr, &mStructedBuffer);
    if (FAILED(hr)) { return false; }

    return true;
}

RSPass_FromTex::RSPass_FromTex(
    std::string& _name, PASS_TYPE _type, RSRoot_DX11* _root) :
    RSPass_Base(_name, _type, _root),
    mVertexShader(nullptr), mPixelShader(nullptr),
    //mRasterizerState(nullptr), mDepthStencilState(nullptr),
    mSwapChainRtv(nullptr), mInputSrv(nullptr),
    mSampler(nullptr),
    mIndexBuffer(nullptr), mLayout(nullptr),
    mTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP)
{

}

RSPass_FromTex::RSPass_FromTex(
    const RSPass_FromTex& _source) :
    RSPass_Base(_source),
    mVertexShader(_source.mVertexShader),
    mPixelShader(_source.mPixelShader),
    //mRasterizerState(_source.mRasterizerState),
    //mDepthStencilState(_source.mDepthStencilState),
    mSwapChainRtv(_source.mSwapChainRtv),
    mInputSrv(_source.mInputSrv),
    mSampler(_source.mSampler),
    mIndexBuffer(_source.mIndexBuffer),
    mLayout(_source.mLayout), mTopology(_source.mTopology)
{

}

RSPass_FromTex::~RSPass_FromTex()
{

}

RSPass_FromTex* RSPass_FromTex::ClonePass()
{
    return new RSPass_FromTex(*this);
}

bool RSPass_FromTex::InitPass()
{
    if (!CreateBuffer()) { return false; }
    if (!CreateShaders()) { return false; }
    if (!CreateViews()) { return false; }
    if (!CreateSamplers()) { return false; }

    return true;
}

void RSPass_FromTex::ReleasePass()
{
    RS_RELEASE(mVertexShader);
    RS_RELEASE(mPixelShader);
    RS_RELEASE(mSampler);
    //RS_RELEASE(mSwapChainRtv);
    //RS_RELEASE(mInputSrv);
    std::string name = "temp-tex-depth";
    g_Root->TexturesManager()->DeleteDataTex(name);
}

void RSPass_FromTex::ExecuatePass()
{
    STContext()->OMSetRenderTargets(1, &mSwapChainRtv, nullptr);
    STContext()->RSSetViewports(1, &g_ViewPort);
    STContext()->ClearRenderTargetView(
        mSwapChainRtv, DirectX::Colors::DarkGreen);
    STContext()->IASetIndexBuffer(mIndexBuffer,
        DXGI_FORMAT_R32_UINT, 0);
    STContext()->VSSetShader(mVertexShader, nullptr, 0);
    STContext()->PSSetShader(mPixelShader, nullptr, 0);
    STContext()->PSSetSamplers(0, 1, &mSampler);
    STContext()->PSSetShaderResources(0, 1, &mInputSrv);
    STContext()->RSSetState(nullptr);

    STContext()->DrawIndexedInstanced(6, 1, 0, 0, 0);

    ID3D11ShaderResourceView* null = nullptr;
    STContext()->PSSetShaderResources(0, 1, &null);
}

bool RSPass_FromTex::CreateBuffer()
{
    HRESULT hr = S_OK;
    D3D11_BUFFER_DESC bufDesc = {};
    std::string name = "";

    UINT indices[6] =
    {
        0, 1, 2,
        0, 2, 3
    };
    bufDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufDesc.ByteWidth = sizeof(UINT) * 6;
    bufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufDesc.CPUAccessFlags = 0;
    bufDesc.StructureByteStride = 0;
    bufDesc.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData = {};
    ZeroMemory(&iinitData, sizeof(iinitData));
    iinitData.pSysMem = indices;
    hr = Device()->CreateBuffer(
        &bufDesc, &iinitData, &mIndexBuffer);
    if (FAILED(hr)) { return false; }

    mTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    return true;
}

bool RSPass_FromTex::CreateShaders()
{
    ID3DBlob* shaderBlob = nullptr;
    HRESULT hr = S_OK;

    hr = Tool::CompileShaderFromFile(
        L".\\Shaders\\fromtex_vertex.hlsl",
        "main", "vs_5_0", &shaderBlob);
    if (FAILED(hr)) { return false; }

    hr = Device()->CreateVertexShader(
        shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(),
        nullptr, &mVertexShader);
    shaderBlob->Release();
    shaderBlob = nullptr;
    if (FAILED(hr)) { return false; }

    hr = Tool::CompileShaderFromFile(
        L".\\Shaders\\fromtex_pixel.hlsl",
        "main", "ps_5_0", &shaderBlob);
    if (FAILED(hr)) { return false; }

    hr = Device()->CreatePixelShader(
        shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(),
        nullptr, &mPixelShader);
    shaderBlob->Release();
    shaderBlob = nullptr;
    if (FAILED(hr)) { return false; }

    return true;
}

bool RSPass_FromTex::CreateViews()
{
    std::string name = "temp-tex";
    mSwapChainRtv = g_Root->Devices()->GetSwapChainRtv();
    auto info = g_Root->TexturesManager()->GetDataTexInfo(name);
    if (!info) { return false; }
    mInputSrv = info->mSrv;
    if (!mInputSrv) { return false; }

    return true;
}

bool RSPass_FromTex::CreateSamplers()
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

    hr = Device()->CreateSamplerState(
        &sampDesc, &mSampler);
    if (FAILED(hr)) { return false; }

    return true;
}
