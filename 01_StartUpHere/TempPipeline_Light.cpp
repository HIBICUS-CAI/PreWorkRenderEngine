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
    std::string name = "basic-light";
    RSPass_Light* light = new RSPass_Light(
        name, PASS_TYPE::RENDER, g_Root);

    light->SetExecuateOrder(1);

    name = "light-topic";
    RSTopic* topic = new RSTopic(name);
    topic->StartTopicAssembly();
    topic->InsertPass(light);
    topic->SetExecuateOrder(1);
    topic->FinishTopicAssembly();

    name = "light-pipeline";
    g_TempPipeline = new RSPipeline(name);
    g_TempPipeline->StartPipelineAssembly();
    g_TempPipeline->InsertTopic(topic);
    g_TempPipeline->FinishPipelineAssembly();

    if (!g_TempPipeline->InitAllTopics()) { return false; }

    name = g_TempPipeline->GetPipelineName();
    g_Root->PipelinesManager()->AddPipeline(
        name, g_TempPipeline);
    g_Root->PipelinesManager()->SetPipeline(name);
    g_Root->PipelinesManager()->ProcessNextPipeline();

    return true;
}

RSPass_Light::RSPass_Light(
    std::string& _name, PASS_TYPE _type, RSRoot_DX11* _root) :
    RSPass_Base(_name, _type, _root),
    mVertexShader(nullptr), mPixelShader(nullptr),
    mRasterizerState(nullptr), mDepthStencilView(nullptr),
    mMeshTexSampler(nullptr), mRenderTargetView(nullptr),
    mDrawCallType(DRAWCALL_TYPE::OPACITY),
    mDrawCallPipe(nullptr),
    mViewProjStructedBuffer(nullptr),
    mViewProjStructedBufferSrv(nullptr),
    mInstanceStructedBuffer(nullptr),
    mInstanceStructedBufferSrv(nullptr),
    mLightStructedBuffer(nullptr),
    mLightStructedBufferSrv(nullptr),
    mLightInfoStructedBuffer(nullptr),
    mLightInfoStructedBufferSrv(nullptr),
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
    mRenderTargetView(_source.mRenderTargetView),
    mMeshTexSampler(_source.mMeshTexSampler),
    mDrawCallType(_source.mDrawCallType),
    mDrawCallPipe(_source.mDrawCallPipe),
    mViewProjStructedBuffer(_source.mViewProjStructedBuffer),
    mViewProjStructedBufferSrv(_source.mViewProjStructedBufferSrv),
    mInstanceStructedBuffer(_source.mInstanceStructedBuffer),
    mInstanceStructedBufferSrv(_source.mInstanceStructedBufferSrv),
    mLightStructedBuffer(_source.mLightStructedBuffer),
    mLightStructedBufferSrv(_source.mLightStructedBufferSrv),
    mLightInfoStructedBuffer(_source.mLightInfoStructedBuffer),
    mLightInfoStructedBufferSrv(_source.mLightInfoStructedBufferSrv),
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

    std::string name = "tex-depth-light";
    g_Root->TexturesManager()->DeleteDataTex(name);
}

void RSPass_Light::ExecuatePass()
{
    STContext()->OMSetRenderTargets(1,
        &mRenderTargetView, mDepthStencilView);
    STContext()->ClearRenderTargetView(
        mRenderTargetView, DirectX::Colors::DarkGreen);
    STContext()->ClearDepthStencilView(
        mDepthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);
    STContext()->VSSetShader(mVertexShader, nullptr, 0);
    STContext()->PSSetShader(mPixelShader, nullptr, 0);
    STContext()->PSSetSamplers(0, 1, &mMeshTexSampler);
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
        STContext()->Map(mInstanceStructedBuffer, 0,
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
        STContext()->Unmap(mInstanceStructedBuffer, 0);

        STContext()->Map(mViewProjStructedBuffer, 0,
            D3D11_MAP_WRITE_DISCARD, 0, &msr);
        ViewProj* vp_data = (ViewProj*)msr.pData;
        mat = DirectX::XMLoadFloat4x4(
            &call.mCameraData.mViewMat);
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&vp_data[0].mViewMat, mat);
        mat = DirectX::XMLoadFloat4x4(
            &call.mCameraData.mProjMat);
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&vp_data[0].mProjMat, mat);
        STContext()->Unmap(mViewProjStructedBuffer, 0);

        STContext()->IASetInputLayout(
            call.mMeshData.mLayout);
        STContext()->IASetPrimitiveTopology(
            call.mMeshData.mTopologyType);
        STContext()->IASetVertexBuffers(
            0, 1, &call.mMeshData.mVertexBuffer,
            &stride, &offset);
        STContext()->IASetIndexBuffer(
            call.mMeshData.mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
        STContext()->VSSetShaderResources(
            0, 1, &mViewProjStructedBufferSrv);
        STContext()->VSSetShaderResources(
            1, 1, &mInstanceStructedBufferSrv);
        STContext()->PSSetShaderResources(
            0, 1, &call.mTextureDatas[0].mSrv);

        STContext()->DrawIndexedInstanced(
            call.mMeshData.mIndexCount,
            (UINT)call.mInstanceData.mDataPtr->size(), 0, 0, 0);
    }

    // TEMP-----------------------------
    g_Root->Devices()->PresentSwapChain();
    mDrawCallPipe->mDatas.clear();

    ID3D11RenderTargetView* null = nullptr;
    STContext()->OMSetRenderTargets(1, &null, nullptr);
}

bool RSPass_Light::CreateShaders()
{
    ID3DBlob* shaderBlob = nullptr;
    HRESULT hr = S_OK;

    hr = Tool::CompileShaderFromFile(
        L".\\Shaders\\light_vertex.hlsl",
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
        L".\\Shaders\\light_pixel.hlsl",
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

bool RSPass_Light::CreateStates()
{
    return true;
}

bool RSPass_Light::CreateBuffers()
{
    HRESULT hr = S_OK;
    D3D11_BUFFER_DESC bdc = {};

    ZeroMemory(&bdc, sizeof(bdc));
    bdc.Usage = D3D11_USAGE_DYNAMIC;
    bdc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bdc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    bdc.ByteWidth = 256 * sizeof(RS_LIGHT_INFO);
    bdc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bdc.StructureByteStride = sizeof(RS_LIGHT_INFO);
    hr = Device()->CreateBuffer(
        &bdc, nullptr, &mLightStructedBuffer);
    if (FAILED(hr)) { return false; }

    bdc.ByteWidth = 256 * sizeof(RS_INSTANCE_DATA);
    bdc.StructureByteStride = sizeof(RS_INSTANCE_DATA);
    hr = Device()->CreateBuffer(
        &bdc, nullptr, &mInstanceStructedBuffer);
    if (FAILED(hr)) { return false; }

    bdc.ByteWidth = sizeof(ViewProj);
    bdc.StructureByteStride = sizeof(ViewProj);
    hr = Device()->CreateBuffer(
        &bdc, nullptr, &mViewProjStructedBuffer);
    if (FAILED(hr)) { return false; }

    bdc.ByteWidth = sizeof(Ambient);
    bdc.StructureByteStride = sizeof(Ambient);
    hr = Device()->CreateBuffer(
        &bdc, nullptr, &mAmbientStructedBuffer);
    if (FAILED(hr)) { return false; }

    bdc.ByteWidth = sizeof(LightInfo);
    bdc.StructureByteStride = sizeof(LightInfo);
    hr = Device()->CreateBuffer(
        &bdc, nullptr, &mLightInfoStructedBuffer);
    if (FAILED(hr)) { return false; }

    return true;
}

bool RSPass_Light::CreateViews()
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
    std::string name = "tex-depth-light";
    dti.mTexture = depthTex;
    dti.mDsv = mDepthStencilView;
    g_Root->TexturesManager()->AddDataTexture(name, dti);

    D3D11_SHADER_RESOURCE_VIEW_DESC desSRV = {};
    ZeroMemory(&desSRV, sizeof(desSRV));
    desSRV.Format = DXGI_FORMAT_UNKNOWN;
    desSRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    desSRV.Buffer.ElementWidth = 256;
    hr = Device()->CreateShaderResourceView(
        mInstanceStructedBuffer,
        &desSRV, &mInstanceStructedBufferSrv);
    if (FAILED(hr)) { return false; }

    hr = Device()->CreateShaderResourceView(
        mLightStructedBuffer,
        &desSRV, &mLightStructedBufferSrv);
    if (FAILED(hr)) { return false; }

    desSRV.Buffer.ElementWidth = 1;
    hr = Device()->CreateShaderResourceView(
        mViewProjStructedBuffer,
        &desSRV, &mViewProjStructedBufferSrv);
    if (FAILED(hr)) { return false; }

    hr = Device()->CreateShaderResourceView(
        mLightInfoStructedBuffer,
        &desSRV, &mLightInfoStructedBufferSrv);
    if (FAILED(hr)) { return false; }

    hr = Device()->CreateShaderResourceView(
        mAmbientStructedBuffer,
        &desSRV, &mAmbientStructedBufferSrv);
    if (FAILED(hr)) { return false; }

    return true;
}

bool RSPass_Light::CreateSamplers()
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
        &sampDesc, &mMeshTexSampler);
    if (FAILED(hr)) { return false; }

    return true;
}
