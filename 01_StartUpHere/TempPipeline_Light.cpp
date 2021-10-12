#include "TempPipeline_Light.h"
#include "RSRoot_DX11.h"
#include "RSPipeline.h"
#include "RSTopic.h"
#include "RSShaderCompile.h"
#include "RSDevices.h"
#include "RSDrawCallsPool.h"
#include "RSStaticResources.h"
#include "RSPipelinesManager.h"
#include "RSCamera.h"
#include "RSLight.h"
#include "RSCamerasContainer.h"
#include "RSLightsContainer.h"
#include <DirectXColors.h>
#include <DirectXPackedVector.h>
#include <DirectXTK\DDSTextureLoader.h>
#include <cstdlib>
#include <ctime>
#include "TempMesh.h"
#include "TempRenderConfig.h"

#define RS_RELEASE(p) { if (p) { (p)->Release(); (p)=nullptr; } }
static RSRoot_DX11* g_Root = nullptr;
static RSPipeline* g_TempPipeline = nullptr;
static D3D11_VIEWPORT g_ViewPort = {};
static bool g_RenderInDeferred = true;

bool CreateTempLightPipeline()
{
    g_Root = GetRSRoot_DX11_Singleton();

    std::string name = "basic-light";
    RSPass_Light* light = new RSPass_Light(
        name, PASS_TYPE::RENDER, g_Root);
    light->SetExecuateOrder(1);

    name = "defered-light";
    RSPass_Defered* defered = new RSPass_Defered(
        name, PASS_TYPE::RENDER, g_Root);
    defered->SetExecuateOrder(1);

    name = "basic-shadowmap";
    RSPass_Shadow* shadow = new RSPass_Shadow(
        name, PASS_TYPE::RENDER, g_Root);
    shadow->SetExecuateOrder(1);

    name = "basic-ssao";
    RSPass_Ssao* ssao = new RSPass_Ssao(
        name, PASS_TYPE::RENDER, g_Root);
    ssao->SetExecuateOrder(2);

    name = "kbblur-ssao";
    RSPass_KBBlur* kbblur = new RSPass_KBBlur(
        name, PASS_TYPE::COMPUTE, g_Root);
    kbblur->SetExecuateOrder(3);

    name = "sky-skysphere";
    RSPass_SkyShpere* skysphere = new RSPass_SkyShpere(
        name, PASS_TYPE::RENDER, g_Root);
    skysphere->SetExecuateOrder(1);

    name = "sprite-ui";
    RSPass_Sprite* sprite = new RSPass_Sprite(
        name, PASS_TYPE::RENDER, g_Root);
    sprite->SetExecuateOrder(1);

    name = "mrt-pass";
    RSPass_MRT* mrt = new RSPass_MRT(
        name, PASS_TYPE::RENDER, g_Root);
    mrt->SetExecuateOrder(1);

    name = "mrt-topic";
    RSTopic* mrt_topic = new RSTopic(name);
    mrt_topic->StartTopicAssembly();
    mrt_topic->InsertPass(mrt);
    mrt_topic->SetExecuateOrder(1);
    mrt_topic->FinishTopicAssembly();

    name = "sprite-topic";
    RSTopic* sprite_topic = new RSTopic(name);
    sprite_topic->StartTopicAssembly();
    sprite_topic->InsertPass(sprite);
    sprite_topic->SetExecuateOrder(6);
    sprite_topic->FinishTopicAssembly();

    name = "skysphere-topic";
    RSTopic* sky_topic = new RSTopic(name);
    sky_topic->StartTopicAssembly();
    sky_topic->InsertPass(skysphere);
    sky_topic->SetExecuateOrder(5);
    sky_topic->FinishTopicAssembly();

    name = "ssao-topic";
    RSTopic* ssao_topic = new RSTopic(name);
    ssao_topic->StartTopicAssembly();
    ssao_topic->InsertPass(ssao);
    ssao_topic->InsertPass(kbblur);
    ssao_topic->SetExecuateOrder(2);
    ssao_topic->FinishTopicAssembly();

    name = "shadowmap-topic";
    RSTopic* shadow_topic = new RSTopic(name);
    shadow_topic->StartTopicAssembly();
    shadow_topic->InsertPass(shadow);
    shadow_topic->SetExecuateOrder(3);
    shadow_topic->FinishTopicAssembly();

    name = "light-topic";
    RSTopic* light_topic = new RSTopic(name);
    light_topic->StartTopicAssembly();
    light_topic->InsertPass(light);
    light_topic->SetExecuateOrder(4);
    light_topic->FinishTopicAssembly();

    name = "defered-light-topic";
    RSTopic* defered_topic = new RSTopic(name);
    defered_topic->StartTopicAssembly();
    defered_topic->InsertPass(defered);
    defered_topic->SetExecuateOrder(4);
    defered_topic->FinishTopicAssembly();

    name = "light-pipeline";
    g_TempPipeline = new RSPipeline(name);
    g_TempPipeline->StartPipelineAssembly();
    g_TempPipeline->InsertTopic(shadow_topic);
    if (GetRenderConfig().mDeferredRenderingEnable)
    {
        g_TempPipeline->InsertTopic(defered_topic);
        g_RenderInDeferred = true;
    }
    else
    {
        g_TempPipeline->InsertTopic(light_topic);
        g_RenderInDeferred = false;
    }
    g_TempPipeline->InsertTopic(ssao_topic);
    g_TempPipeline->InsertTopic(sky_topic);
    g_TempPipeline->InsertTopic(sprite_topic);
    g_TempPipeline->InsertTopic(mrt_topic);
    g_TempPipeline->FinishPipelineAssembly();

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

RSPass_Light::RSPass_Light(
    std::string& _name, PASS_TYPE _type, RSRoot_DX11* _root) :
    RSPass_Base(_name, _type, _root),
    mVertexShader(nullptr), mPixelShader(nullptr),
    mRasterizerState(nullptr), mDepthStencilView(nullptr),
    mMeshTexSampler(nullptr), mRenderTargetView(nullptr),
    mShadowTexSampler(nullptr),
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
    mAmbientStructedBufferSrv(nullptr),
    mMaterialStructedBuffer(nullptr),
    mMaterialStructedBufferSrv(nullptr),
    mShadowStructedBuffer(nullptr),
    mShadowStructedBufferSrv(nullptr),
    mSsaoSrv(nullptr), mDepthStencilState(nullptr),
    mRSCameraInfo(nullptr)
{

}

RSPass_Light::RSPass_Light(const RSPass_Light& _source) :
    RSPass_Base(_source),
    mVertexShader(_source.mVertexShader),
    mPixelShader(_source.mPixelShader),
    mRasterizerState(_source.mRasterizerState),
    mDepthStencilState(_source.mDepthStencilState),
    mDepthStencilView(_source.mDepthStencilView),
    mRenderTargetView(_source.mRenderTargetView),
    mMeshTexSampler(_source.mMeshTexSampler),
    mShadowTexSampler(_source.mShadowTexSampler),
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
    mAmbientStructedBufferSrv(_source.mAmbientStructedBufferSrv),
    mMaterialStructedBuffer(_source.mMaterialStructedBuffer),
    mMaterialStructedBufferSrv(_source.mMaterialStructedBufferSrv),
    mShadowStructedBuffer(_source.mShadowStructedBuffer),
    mShadowStructedBufferSrv(_source.mShadowStructedBufferSrv),
    mSsaoSrv(_source.mSsaoSrv),
    mRSCameraInfo(_source.mRSCameraInfo)
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

    std::string name = "temp-cam";
    mRSCameraInfo = g_Root->CamerasContainer()->
        GetRSCameraInfo(name);

    return true;
}

void RSPass_Light::ReleasePass()
{
    RS_RELEASE(mVertexShader);
    RS_RELEASE(mPixelShader);
    RS_RELEASE(mRasterizerState);
    RS_RELEASE(mDepthStencilState);
    RS_RELEASE(mMeshTexSampler);
    RS_RELEASE(mShadowTexSampler);
    RS_RELEASE(mViewProjStructedBufferSrv);
    RS_RELEASE(mViewProjStructedBuffer);
    RS_RELEASE(mInstanceStructedBufferSrv);
    RS_RELEASE(mInstanceStructedBuffer);
    RS_RELEASE(mLightStructedBufferSrv);
    RS_RELEASE(mLightStructedBuffer);
    RS_RELEASE(mAmbientStructedBufferSrv);
    RS_RELEASE(mAmbientStructedBuffer);
}

void RSPass_Light::ExecuatePass()
{
    STContext()->OMSetRenderTargets(1,
        &mRenderTargetView, mDepthStencilView);
    STContext()->RSSetViewports(1, &g_ViewPort);
    STContext()->ClearRenderTargetView(
        mRenderTargetView, DirectX::Colors::DarkGreen);
    STContext()->VSSetShader(mVertexShader, nullptr, 0);
    STContext()->PSSetShader(mPixelShader, nullptr, 0);
    STContext()->PSSetSamplers(0, 1, &mMeshTexSampler);
    STContext()->PSSetSamplers(1, 1, &mShadowTexSampler);
    STContext()->RSSetState(mRasterizerState);
    STContext()->OMSetDepthStencilState(mDepthStencilState, 0);

    DirectX::XMMATRIX mat = {};
    DirectX::XMFLOAT4X4 flt44 = {};
    UINT stride = sizeof(VERTEX_INFO);
    UINT offset = 0;

    D3D11_MAPPED_SUBRESOURCE msr = {};
    STContext()->Map(mViewProjStructedBuffer, 0,
        D3D11_MAP_WRITE_DISCARD, 0, &msr);
    ViewProj* vp_data = (ViewProj*)msr.pData;
    mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mViewMat);
    mat = DirectX::XMMatrixTranspose(mat);
    DirectX::XMStoreFloat4x4(&vp_data[0].mViewMat, mat);
    mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mProjMat);
    mat = DirectX::XMMatrixTranspose(mat);
    DirectX::XMStoreFloat4x4(&vp_data[0].mProjMat, mat);
    STContext()->Unmap(mViewProjStructedBuffer, 0);

    STContext()->Map(mAmbientStructedBuffer, 0,
        D3D11_MAP_WRITE_DISCARD, 0, &msr);
    Ambient* amb_data = (Ambient*)msr.pData;
    amb_data[0].mAmbient = { 0.3f,0.3f,0.3f,1.f };
    STContext()->Unmap(mAmbientStructedBuffer, 0);

    static auto lights = g_Root->LightsContainer()->GetLights();
    STContext()->Map(mLightInfoStructedBuffer, 0,
        D3D11_MAP_WRITE_DISCARD, 0, &msr);
    LightInfo* li_data = (LightInfo*)msr.pData;
    li_data[0].mCameraPos = mRSCameraInfo->mEyePosition;
    UINT dNum = 0;
    UINT sNum = 0;
    UINT pNum = 0;
    for (auto& l : *lights)
    {
        auto type = l->GetRSLightType();
        switch (type)
        {
        case LIGHT_TYPE::DIRECT:
            ++dNum; break;
        case LIGHT_TYPE::POINT:
            ++pNum; break;
        case LIGHT_TYPE::SPOT:
            ++sNum; break;
        default: break;
        }
    }
    li_data[0].mDirectLightNum = dNum;
    li_data[0].mPointLightNum = pNum;
    li_data[0].mSpotLightNum = sNum;
    li_data[0].mShadowLightNum = (UINT)g_Root->LightsContainer()->
        GetShadowLights()->size();
    li_data[0].mShadowLightIndex[0] = -1;
    li_data[0].mShadowLightIndex[1] = -1;
    li_data[0].mShadowLightIndex[2] = -1;
    li_data[0].mShadowLightIndex[3] = -1;
    auto shadowIndeices = g_Root->LightsContainer()->
        GetShadowLightIndeices();
    for (UINT i = 0; i < li_data[0].mShadowLightNum; i++)
    {
        li_data[0].mShadowLightIndex[i] = (*shadowIndeices)[i];
        if (i >= 3)
        {
            break;
        }
    }
    STContext()->Unmap(mLightInfoStructedBuffer, 0);

    STContext()->Map(mLightStructedBuffer, 0,
        D3D11_MAP_WRITE_DISCARD, 0, &msr);
    RS_LIGHT_INFO* l_data = (RS_LIGHT_INFO*)msr.pData;
    UINT lightIndex = 0;
    for (auto& l : *lights)
    {
        l_data[lightIndex++] = *(l->GetRSLightInfo());
    }
    STContext()->Unmap(mLightStructedBuffer, 0);

    STContext()->Map(mShadowStructedBuffer, 0,
        D3D11_MAP_WRITE_DISCARD, 0, &msr);
    ShadowInfo* s_data = (ShadowInfo*)msr.pData;
    // TEMP---------------------
    auto shadowLights = g_Root->LightsContainer()->
        GetShadowLights();
    UINT shadowSize = (UINT)shadowLights->size();
    for (UINT i = 0; i < shadowSize; i++)
    {
        auto lcam = (*shadowLights)[i]->GetRSLightCamera();
        mat = DirectX::XMLoadFloat4x4(
            &(lcam->GetRSCameraInfo()->mViewMat));
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&s_data[i].mShadowViewMat, mat);
        mat = DirectX::XMLoadFloat4x4(
            &(lcam->GetRSCameraInfo()->mProjMat));
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&s_data[i].mShadowProjMat, mat);
    }

    static DirectX::XMMATRIX T(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f);
    mat = DirectX::XMMatrixTranspose(
        DirectX::XMLoadFloat4x4(&mRSCameraInfo->mViewMat) *
        DirectX::XMLoadFloat4x4(&mRSCameraInfo->mProjMat) *
        T);
    DirectX::XMStoreFloat4x4(&s_data[0].mSSAOMat, mat);
    // TEMP---------------------
    STContext()->Unmap(mShadowStructedBuffer, 0);

    STContext()->VSSetShaderResources(
        0, 1, &mViewProjStructedBufferSrv);
    STContext()->VSSetShaderResources(
        2, 1, &mShadowStructedBufferSrv);
    STContext()->PSSetShaderResources(
        0, 1, &mAmbientStructedBufferSrv);
    STContext()->PSSetShaderResources(
        1, 1, &mLightInfoStructedBufferSrv);
    STContext()->PSSetShaderResources(
        3, 1, &mLightStructedBufferSrv);
    static std::string depthtex = "light-depth-light-other";
    STContext()->PSSetShaderResources(
        5, 1, &g_Root->TexturesManager()->
        GetDataTexInfo(depthtex)->mSrv);
    STContext()->PSSetShaderResources(
        6, 1, &mSsaoSrv);

    for (auto& call : mDrawCallPipe->mDatas)
    {
        auto vecPtr = call.mInstanceData.mDataPtr;
        auto size = vecPtr->size();
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

        STContext()->Map(mMaterialStructedBuffer, 0,
            D3D11_MAP_WRITE_DISCARD, 0, &msr);
        RS_MATERIAL_INFO* m_data = (RS_MATERIAL_INFO*)msr.pData;
        m_data[0] = call.mMaterialData;
        STContext()->Unmap(mMaterialStructedBuffer, 0);

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
            1, 1, &mInstanceStructedBufferSrv);
        STContext()->PSSetShaderResources(
            2, 1, &mMaterialStructedBufferSrv);
        STContext()->PSSetShaderResources(
            4, 1, &call.mTextureDatas[0].mSrv);

        if (call.mTextureDatas[1].mUse)
        {
            STContext()->PSSetShaderResources(
                7, 1, &(call.mTextureDatas[1].mSrv));
        }

        STContext()->DrawIndexedInstanced(
            call.mMeshData.mIndexCount,
            (UINT)call.mInstanceData.mDataPtr->size(), 0, 0, 0);
    }

    ID3D11RenderTargetView* null = nullptr;
    ID3D11ShaderResourceView* nullsrv = nullptr;
    STContext()->OMSetRenderTargets(1, &null, nullptr);
    STContext()->PSSetShaderResources(0, 1, &nullsrv);
    STContext()->PSSetShaderResources(1, 1, &nullsrv);
    STContext()->PSSetShaderResources(2, 1, &nullsrv);
    STContext()->PSSetShaderResources(3, 1, &nullsrv);
    STContext()->PSSetShaderResources(4, 1, &nullsrv);
    STContext()->PSSetShaderResources(5, 1, &nullsrv);
    STContext()->PSSetShaderResources(6, 1, &nullsrv);
    STContext()->PSSetShaderResources(7, 1, &nullsrv);
    STContext()->OMSetDepthStencilState(nullptr, 0);
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
    HRESULT hr = S_OK;
    D3D11_DEPTH_STENCIL_DESC depDesc = {};

    depDesc.DepthEnable = TRUE;
    depDesc.DepthFunc = D3D11_COMPARISON_EQUAL;
    hr = Device()->CreateDepthStencilState(&depDesc,
        &mDepthStencilState);
    if (FAILED(hr)) { return false; }

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
    bdc.ByteWidth = MAX_LIGHT_SIZE * sizeof(RS_LIGHT_INFO);
    bdc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bdc.StructureByteStride = sizeof(RS_LIGHT_INFO);
    hr = Device()->CreateBuffer(
        &bdc, nullptr, &mLightStructedBuffer);
    if (FAILED(hr)) { return false; }

    bdc.ByteWidth = MAX_INSTANCE_SIZE * sizeof(RS_INSTANCE_DATA);
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

    bdc.ByteWidth = sizeof(RS_MATERIAL_INFO);
    bdc.StructureByteStride = sizeof(RS_MATERIAL_INFO);
    hr = Device()->CreateBuffer(
        &bdc, nullptr, &mMaterialStructedBuffer);
    if (FAILED(hr)) { return false; }

    bdc.ByteWidth = MAX_SHADOW_SIZE * sizeof(ShadowInfo);
    bdc.StructureByteStride = sizeof(ShadowInfo);
    hr = Device()->CreateBuffer(
        &bdc, nullptr, &mShadowStructedBuffer);
    if (FAILED(hr)) { return false; }

    return true;
}

bool RSPass_Light::CreateViews()
{
    mRenderTargetView = g_Root->Devices()->GetSwapChainRtv();

    HRESULT hr = S_OK;
    std::string name = "mrt-depth";
    mDepthStencilView = g_Root->TexturesManager()->
        GetDataTexInfo(name)->mDsv;

    D3D11_SHADER_RESOURCE_VIEW_DESC desSRV = {};
    ZeroMemory(&desSRV, sizeof(desSRV));
    desSRV.Format = DXGI_FORMAT_UNKNOWN;
    desSRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    desSRV.Buffer.ElementWidth = MAX_INSTANCE_SIZE;
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

    hr = Device()->CreateShaderResourceView(
        mMaterialStructedBuffer,
        &desSRV, &mMaterialStructedBufferSrv);
    if (FAILED(hr)) { return false; }

    desSRV.Buffer.ElementWidth = MAX_SHADOW_SIZE;
    hr = Device()->CreateShaderResourceView(
        mShadowStructedBuffer,
        &desSRV, &mShadowStructedBufferSrv);
    if (FAILED(hr)) { return false; }

    name = "ssao-tex-ssao";
    mSsaoSrv = g_Root->TexturesManager()->
        GetDataTexInfo(name)->mSrv;

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

    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    hr = Device()->CreateSamplerState(
        &sampDesc, &mShadowTexSampler);
    if (FAILED(hr)) { return false; }

    return true;
}

RSPass_Shadow::RSPass_Shadow(
    std::string& _name, PASS_TYPE _type, RSRoot_DX11* _root) :
    RSPass_Base(_name, _type, _root),
    mVertexShader(nullptr),
    mRasterizerState(nullptr),
    mDepthStencilView({ nullptr,nullptr,nullptr,nullptr }),
    mDrawCallType(DRAWCALL_TYPE::OPACITY),
    mDrawCallPipe(nullptr),
    mViewProjStructedBuffer(nullptr),
    mViewProjStructedBufferSrv(nullptr),
    mInstanceStructedBuffer(nullptr),
    mInstanceStructedBufferSrv(nullptr)
{

}

RSPass_Shadow::RSPass_Shadow(const RSPass_Shadow& _source) :
    RSPass_Base(_source),
    mVertexShader(_source.mVertexShader),
    mRasterizerState(_source.mRasterizerState),
    mDepthStencilView(_source.mDepthStencilView),
    mDrawCallType(_source.mDrawCallType),
    mDrawCallPipe(_source.mDrawCallPipe),
    mViewProjStructedBuffer(_source.mViewProjStructedBuffer),
    mViewProjStructedBufferSrv(_source.mViewProjStructedBufferSrv),
    mInstanceStructedBuffer(_source.mInstanceStructedBuffer),
    mInstanceStructedBufferSrv(_source.mInstanceStructedBufferSrv)
{

}

RSPass_Shadow::~RSPass_Shadow()
{

}

RSPass_Shadow* RSPass_Shadow::ClonePass()
{
    return new RSPass_Shadow(*this);
}

bool RSPass_Shadow::InitPass()
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

void RSPass_Shadow::ReleasePass()
{
    RS_RELEASE(mVertexShader);
    RS_RELEASE(mRasterizerState);
    RS_RELEASE(mViewProjStructedBufferSrv);
    RS_RELEASE(mViewProjStructedBuffer);
    RS_RELEASE(mInstanceStructedBufferSrv);
    RS_RELEASE(mInstanceStructedBuffer);

    std::string name = "light-depth-light-other";
    g_Root->TexturesManager()->DeleteDataTex(name);
    name = "light-depth-light-dep0";
    g_Root->TexturesManager()->DeleteDataTex(name);
    name = "light-depth-light-dep1";
    g_Root->TexturesManager()->DeleteDataTex(name);
    name = "light-depth-light-dep2";
    g_Root->TexturesManager()->DeleteDataTex(name);
    name = "light-depth-light-dep3";
    g_Root->TexturesManager()->DeleteDataTex(name);
}

void RSPass_Shadow::ExecuatePass()
{
    ID3D11RenderTargetView* null = nullptr;
    STContext()->VSSetShader(mVertexShader, nullptr, 0);
    STContext()->PSSetShader(nullptr, nullptr, 0);
    STContext()->RSSetState(mRasterizerState);

    DirectX::XMMATRIX mat = {};
    DirectX::XMFLOAT4X4 flt44 = {};
    UINT stride = sizeof(VERTEX_INFO);
    UINT offset = 0;
    auto shadowLights = g_Root->LightsContainer()->
        GetShadowLights();
    UINT shadowSize = (UINT)shadowLights->size();
    D3D11_MAPPED_SUBRESOURCE msr = {};

    for (UINT i = 0; i < shadowSize; i++)
    {
        STContext()->OMSetRenderTargets(1,
            &null, mDepthStencilView[i]);
        STContext()->RSSetViewports(1, &g_ViewPort);
        STContext()->ClearDepthStencilView(
            mDepthStencilView[i], D3D11_CLEAR_DEPTH, 1.f, 0);

        STContext()->Map(mViewProjStructedBuffer, 0,
            D3D11_MAP_WRITE_DISCARD, 0, &msr);
        ViewProj* vp_data = (ViewProj*)msr.pData;
        auto light = (*shadowLights)[i];
        auto lcam = light->GetRSLightCamera();
        mat = DirectX::XMLoadFloat4x4(
            &(lcam->GetRSCameraInfo()->mViewMat));
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&vp_data[0].mViewMat, mat);
        mat = DirectX::XMLoadFloat4x4(
            &(lcam->GetRSCameraInfo()->mProjMat));
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&vp_data[0].mProjMat, mat);
        STContext()->Unmap(mViewProjStructedBuffer, 0);

        STContext()->VSSetShaderResources(
            0, 1, &mViewProjStructedBufferSrv);

        for (auto& call : mDrawCallPipe->mDatas)
        {
            auto vecPtr = call.mInstanceData.mDataPtr;
            auto size = vecPtr->size();
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
                1, 1, &mInstanceStructedBufferSrv);

            STContext()->DrawIndexedInstanced(
                call.mMeshData.mIndexCount,
                (UINT)call.mInstanceData.mDataPtr->size(), 0, 0, 0);
        }
    }

    STContext()->OMSetRenderTargets(1, &null, nullptr);
    STContext()->RSSetState(nullptr);
}

bool RSPass_Shadow::CreateShaders()
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

    return true;
}

bool RSPass_Shadow::CreateStates()
{
    HRESULT hr = S_OK;
    D3D11_RASTERIZER_DESC shadowRasterDesc = {};
    ZeroMemory(&shadowRasterDesc, sizeof(shadowRasterDesc));

    shadowRasterDesc.FillMode = D3D11_FILL_SOLID;
    shadowRasterDesc.CullMode = D3D11_CULL_BACK;
    shadowRasterDesc.FrontCounterClockwise = FALSE;
    shadowRasterDesc.DepthBias = 100000;
    shadowRasterDesc.SlopeScaledDepthBias = 1.f;
    shadowRasterDesc.DepthBiasClamp = 0.f;
    shadowRasterDesc.DepthClipEnable = TRUE;
    shadowRasterDesc.ScissorEnable = FALSE;
    shadowRasterDesc.MultisampleEnable = FALSE;
    shadowRasterDesc.AntialiasedLineEnable = FALSE;

    hr = Device()->CreateRasterizerState(&shadowRasterDesc,
        &mRasterizerState);
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

bool RSPass_Shadow::CreateBuffers()
{
    HRESULT hr = S_OK;
    D3D11_BUFFER_DESC bdc = {};

    ZeroMemory(&bdc, sizeof(bdc));
    bdc.Usage = D3D11_USAGE_DYNAMIC;
    bdc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bdc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    bdc.ByteWidth = MAX_INSTANCE_SIZE * sizeof(RS_INSTANCE_DATA);
    bdc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bdc.StructureByteStride = sizeof(RS_INSTANCE_DATA);
    hr = Device()->CreateBuffer(
        &bdc, nullptr, &mInstanceStructedBuffer);
    if (FAILED(hr)) { return false; }

    bdc.ByteWidth = sizeof(ViewProj);
    bdc.StructureByteStride = sizeof(ViewProj);
    hr = Device()->CreateBuffer(
        &bdc, nullptr, &mViewProjStructedBuffer);
    if (FAILED(hr)) { return false; }

    return true;
}

bool RSPass_Shadow::CreateViews()
{
    HRESULT hr = S_OK;
    ID3D11Texture2D* depthTex = nullptr;
    D3D11_TEXTURE2D_DESC texDepSte = {};
    texDepSte.Width = 1280;
    texDepSte.Height = 720;
    texDepSte.MipLevels = 1;
    texDepSte.ArraySize = MAX_SHADOW_SIZE;
    texDepSte.Format = DXGI_FORMAT_R24G8_TYPELESS;
    texDepSte.SampleDesc.Count = 1;
    texDepSte.SampleDesc.Quality = 0;
    texDepSte.Usage = D3D11_USAGE_DEFAULT;
    texDepSte.BindFlags = D3D11_BIND_DEPTH_STENCIL |
        D3D11_BIND_SHADER_RESOURCE;
    texDepSte.CPUAccessFlags = 0;
    texDepSte.MiscFlags = 0;
    hr = Device()->CreateTexture2D(
        &texDepSte, nullptr, &depthTex);
    if (FAILED(hr)) { return false; }

    D3D11_DEPTH_STENCIL_VIEW_DESC desDSV = {};
    desDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    desDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
    desDSV.Texture2DArray.MipSlice = 0;
    desDSV.Texture2DArray.ArraySize = 1;
    for (UINT i = 0; i < MAX_SHADOW_SIZE; i++)
    {
        desDSV.Texture2DArray.FirstArraySlice =
            D3D11CalcSubresource(0, i, 1);
        hr = Device()->CreateDepthStencilView(
            depthTex, &desDSV, &(mDepthStencilView[i]));
        if (FAILED(hr)) { return false; }
    }

    ID3D11ShaderResourceView* srv = nullptr;
    D3D11_SHADER_RESOURCE_VIEW_DESC desSRV = {};
    desSRV.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    desSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    desSRV.Texture2DArray.FirstArraySlice = 0;
    desSRV.Texture2DArray.MostDetailedMip = 0;
    desSRV.Texture2DArray.MipLevels = 1;
    desSRV.Texture2DArray.ArraySize = MAX_SHADOW_SIZE;
    hr = Device()->CreateShaderResourceView(
        depthTex, &desSRV, &srv);
    if (FAILED(hr)) { return false; }

    DATA_TEXTURE_INFO dti = {};
    std::string name = "light-depth-light-other";
    dti.mTexture = depthTex;
    dti.mSrv = srv;
    g_Root->TexturesManager()->AddDataTexture(name, dti);

    dti = {};
    name = "light-depth-light-dep0";
    dti.mDsv = mDepthStencilView[0];
    g_Root->TexturesManager()->AddDataTexture(name, dti);
    name = "light-depth-light-dep1";
    dti.mDsv = mDepthStencilView[1];
    g_Root->TexturesManager()->AddDataTexture(name, dti);
    name = "light-depth-light-dep2";
    dti.mDsv = mDepthStencilView[2];
    g_Root->TexturesManager()->AddDataTexture(name, dti);
    name = "light-depth-light-dep3";
    dti.mDsv = mDepthStencilView[3];
    g_Root->TexturesManager()->AddDataTexture(name, dti);

    ZeroMemory(&desSRV, sizeof(desSRV));
    desSRV.Format = DXGI_FORMAT_UNKNOWN;
    desSRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    desSRV.Buffer.ElementWidth = MAX_INSTANCE_SIZE;
    hr = Device()->CreateShaderResourceView(
        mInstanceStructedBuffer,
        &desSRV, &mInstanceStructedBufferSrv);
    if (FAILED(hr)) { return false; }

    desSRV.Buffer.ElementWidth = 1;
    hr = Device()->CreateShaderResourceView(
        mViewProjStructedBuffer,
        &desSRV, &mViewProjStructedBufferSrv);
    if (FAILED(hr)) { return false; }

    return true;
}

bool RSPass_Shadow::CreateSamplers()
{
    return true;
}

RSPass_Ssao::RSPass_Ssao(
    std::string& _name, PASS_TYPE _type, RSRoot_DX11* _root) :
    RSPass_Base(_name, _type, _root),
    mVertexShader(nullptr), mPixelShader(nullptr),
    mRenderTargetView(nullptr),
    mSsaoInfoStructedBuffer(nullptr),
    mSsaoInfoStructedBufferSrv(nullptr),
    mNormalMapSrv(nullptr),
    mDepthMapSrv(nullptr),
    mRandomMapSrv(nullptr),
    mSamplePointClamp(nullptr), mSampleLinearClamp(nullptr),
    mSampleDepthMap(nullptr), mSampleLinearWrap(nullptr),
    mVertexBuffer(nullptr), mIndexBuffer(nullptr),
    mRSCameraInfo(nullptr)
{
    for (UINT i = 0; i < 14; i++)
    {
        mOffsetVec[i] = {};
    }
}

RSPass_Ssao::RSPass_Ssao(const RSPass_Ssao& _source) :
    RSPass_Base(_source),
    mVertexShader(_source.mVertexShader),
    mPixelShader(_source.mPixelShader),
    mRenderTargetView(_source.mRenderTargetView),
    mSamplePointClamp(_source.mSamplePointClamp),
    mSampleLinearClamp(_source.mSampleLinearClamp),
    mSampleDepthMap(_source.mSampleDepthMap),
    mSampleLinearWrap(_source.mSampleLinearWrap),
    mSsaoInfoStructedBuffer(_source.mSsaoInfoStructedBuffer),
    mSsaoInfoStructedBufferSrv(_source.mSsaoInfoStructedBufferSrv),
    mNormalMapSrv(_source.mNormalMapSrv),
    mDepthMapSrv(_source.mDepthMapSrv),
    mRandomMapSrv(_source.mRandomMapSrv),
    mVertexBuffer(nullptr), mIndexBuffer(nullptr),
    mRSCameraInfo(_source.mRSCameraInfo)
{
    for (UINT i = 0; i < 14; i++)
    {
        mOffsetVec[i] = _source.mOffsetVec[i];
    }
}

RSPass_Ssao::~RSPass_Ssao()
{

}

RSPass_Ssao* RSPass_Ssao::ClonePass()
{
    return new RSPass_Ssao(*this);
}

bool RSPass_Ssao::InitPass()
{
    if (!CreateShaders()) { return false; }
    if (!CreateBuffers()) { return false; }
    if (!CreateTextures()) { return false; }
    if (!CreateViews()) { return false; }
    if (!CreateSamplers()) { return false; }

    DirectX::XMFLOAT4 vec[14] = {};
    vec[0] = DirectX::XMFLOAT4(+1.0f, +1.0f, +1.0f, 0.0f);
    vec[1] = DirectX::XMFLOAT4(-1.0f, -1.0f, -1.0f, 0.0f);
    vec[2] = DirectX::XMFLOAT4(-1.0f, +1.0f, +1.0f, 0.0f);
    vec[3] = DirectX::XMFLOAT4(+1.0f, -1.0f, -1.0f, 0.0f);
    vec[4] = DirectX::XMFLOAT4(+1.0f, +1.0f, -1.0f, 0.0f);
    vec[5] = DirectX::XMFLOAT4(-1.0f, -1.0f, +1.0f, 0.0f);
    vec[6] = DirectX::XMFLOAT4(-1.0f, +1.0f, -1.0f, 0.0f);
    vec[7] = DirectX::XMFLOAT4(+1.0f, -1.0f, +1.0f, 0.0f);
    vec[8] = DirectX::XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.0f);
    vec[9] = DirectX::XMFLOAT4(+1.0f, 0.0f, 0.0f, 0.0f);
    vec[10] = DirectX::XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
    vec[11] = DirectX::XMFLOAT4(0.0f, +1.0f, 0.0f, 0.0f);
    vec[12] = DirectX::XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f);
    vec[13] = DirectX::XMFLOAT4(0.0f, 0.0f, +1.0f, 0.0f);
    const int basic = 25;
    const int range = 75;
    for (int i = 0; i < 14; i++)
    {
        std::srand((unsigned int)std::time(nullptr) +
            (unsigned int)std::rand());
        float s = (float)(std::rand() % range + basic) / 100.f;
        DirectX::XMVECTOR v = DirectX::XMVector4Normalize(
            DirectX::XMLoadFloat4(&vec[i]));
        DirectX::XMStoreFloat4(&vec[i], v);
        vec[i].x *= s;
        vec[i].y *= s;
        vec[i].z *= s;
        mOffsetVec[i] = vec[i];
    }

    std::string name = "temp-cam";
    mRSCameraInfo = g_Root->CamerasContainer()->
        GetRSCameraInfo(name);

    return true;
}

void RSPass_Ssao::ReleasePass()
{
    RS_RELEASE(mVertexShader);
    RS_RELEASE(mPixelShader);
    RS_RELEASE(mSamplePointClamp);
    RS_RELEASE(mSampleLinearClamp);
    RS_RELEASE(mSampleDepthMap);
    RS_RELEASE(mSampleLinearWrap);
    RS_RELEASE(mSsaoInfoStructedBuffer);
    RS_RELEASE(mSsaoInfoStructedBufferSrv);
    RS_RELEASE(mVertexBuffer);
    RS_RELEASE(mIndexBuffer);

    std::string name = "random-tex-ssao";
    g_Root->TexturesManager()->DeleteDataTex(name);
    name = "ssao-tex-ssao";
    g_Root->TexturesManager()->DeleteDataTex(name);
}

void RSPass_Ssao::ExecuatePass()
{
    ID3D11RenderTargetView* null = nullptr;
    ID3D11ShaderResourceView* srvnull = nullptr;
    STContext()->OMSetRenderTargets(1,
        &mRenderTargetView, nullptr);
    STContext()->RSSetViewports(1, &g_ViewPort);
    STContext()->VSSetShader(mVertexShader, nullptr, 0);
    STContext()->PSSetShader(mPixelShader, nullptr, 0);
    STContext()->RSSetState(nullptr);

    DirectX::XMMATRIX mat = {};
    DirectX::XMFLOAT4X4 flt44 = {};
    UINT stride = sizeof(VERTEX_INFO);
    UINT offset = 0;

    D3D11_MAPPED_SUBRESOURCE msr = {};

    STContext()->Map(mSsaoInfoStructedBuffer, 0,
        D3D11_MAP_WRITE_DISCARD, 0, &msr);
    SsaoInfo* ss_data = (SsaoInfo*)msr.pData;

    mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mProjMat);
    mat = DirectX::XMMatrixTranspose(mat);
    DirectX::XMStoreFloat4x4(&ss_data[0].mProj, mat);

    mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mViewMat);
    mat = DirectX::XMMatrixTranspose(mat);
    DirectX::XMStoreFloat4x4(&ss_data[0].mView, mat);

    mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mInvProjMat);
    mat = DirectX::XMMatrixTranspose(mat);
    DirectX::XMStoreFloat4x4(&ss_data[0].mInvProj, mat);

    static DirectX::XMMATRIX T(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f);
    mat = DirectX::XMMatrixTranspose(
        DirectX::XMLoadFloat4x4(&mRSCameraInfo->mProjMat) * T);
    DirectX::XMStoreFloat4x4(&ss_data[0].mTexProj, mat);

    for (UINT i = 0; i < 14; i++)
    {
        ss_data[0].mOffsetVec[i] = mOffsetVec[i];
    }

    ss_data[0].mOcclusionRadius = 0.5f;
    ss_data[0].mOcclusionFadeStart = 0.2f;
    ss_data[0].mOcclusionFadeEnd = 1.0f;
    ss_data[0].mSurfaceEpsilon = 0.05f;
    // TEMP---------------------
    STContext()->Unmap(mSsaoInfoStructedBuffer, 0);

    STContext()->IASetPrimitiveTopology(
        D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    STContext()->IASetVertexBuffers(
        0, 1, &mVertexBuffer,
        &stride, &offset);
    STContext()->IASetIndexBuffer(
        mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    STContext()->VSSetShaderResources(
        0, 1, &mSsaoInfoStructedBufferSrv);
    STContext()->PSSetShaderResources(
        0, 1, &mSsaoInfoStructedBufferSrv);
    STContext()->PSSetShaderResources(
        1, 1, &mNormalMapSrv);
    STContext()->PSSetShaderResources(
        2, 1, &mDepthMapSrv);
    STContext()->PSSetShaderResources(
        3, 1, &mRandomMapSrv);

    STContext()->PSSetSamplers(0, 1, &mSamplePointClamp);
    STContext()->PSSetSamplers(1, 1, &mSampleLinearClamp);
    STContext()->PSSetSamplers(2, 1, &mSampleDepthMap);
    STContext()->PSSetSamplers(3, 1, &mSampleLinearWrap);

    STContext()->DrawIndexedInstanced(6, 1, 0, 0, 0);

    STContext()->OMSetRenderTargets(1, &null, nullptr);
    STContext()->RSSetState(nullptr);
    STContext()->PSSetShaderResources(
        1, 1, &srvnull);
    STContext()->PSSetShaderResources(
        2, 1, &srvnull);
    STContext()->PSSetShaderResources(
        3, 1, &srvnull);
}

bool RSPass_Ssao::CreateShaders()
{
    ID3DBlob* shaderBlob = nullptr;
    HRESULT hr = S_OK;

    hr = Tool::CompileShaderFromFile(
        L".\\Shaders\\ssao_vertex.hlsl",
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
        L".\\Shaders\\ssao_pixel.hlsl",
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

bool RSPass_Ssao::CreateBuffers()
{
    HRESULT hr = S_OK;
    D3D11_BUFFER_DESC bdc = {};

    ZeroMemory(&bdc, sizeof(bdc));
    bdc.Usage = D3D11_USAGE_DYNAMIC;
    bdc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bdc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    bdc.ByteWidth = sizeof(SsaoInfo);
    bdc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bdc.StructureByteStride = sizeof(SsaoInfo);
    hr = Device()->CreateBuffer(
        &bdc, nullptr, &mSsaoInfoStructedBuffer);
    if (FAILED(hr)) { return false; }

    VERTEX_INFO v[4] = {};
    v[0].mPosition = DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f);
    v[1].mPosition = DirectX::XMFLOAT3(-1.0f, +1.0f, 0.0f);
    v[2].mPosition = DirectX::XMFLOAT3(+1.0f, +1.0f, 0.0f);
    v[3].mPosition = DirectX::XMFLOAT3(+1.0f, -1.0f, 0.0f);
    v[0].mNormal = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    v[1].mNormal = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
    v[2].mNormal = DirectX::XMFLOAT3(2.0f, 0.0f, 0.0f);
    v[3].mNormal = DirectX::XMFLOAT3(3.0f, 0.0f, 0.0f);
    v[0].mTangent = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    v[1].mTangent = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
    v[2].mTangent = DirectX::XMFLOAT3(2.0f, 0.0f, 0.0f);
    v[3].mTangent = DirectX::XMFLOAT3(3.0f, 0.0f, 0.0f);
    v[0].mTexCoord = DirectX::XMFLOAT2(0.0f, 1.0f);
    v[1].mTexCoord = DirectX::XMFLOAT2(0.0f, 0.0f);
    v[2].mTexCoord = DirectX::XMFLOAT2(1.0f, 0.0f);
    v[3].mTexCoord = DirectX::XMFLOAT2(1.0f, 1.0f);
    ZeroMemory(&bdc, sizeof(bdc));
    bdc.Usage = D3D11_USAGE_IMMUTABLE;
    bdc.ByteWidth = sizeof(VERTEX_INFO) * 4;
    bdc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bdc.CPUAccessFlags = 0;
    bdc.MiscFlags = 0;
    bdc.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA vinitData = {};
    ZeroMemory(&vinitData, sizeof(vinitData));
    vinitData.pSysMem = v;
    hr = Device()->CreateBuffer(
        &bdc, &vinitData, &mVertexBuffer);
    if (FAILED(hr)) { return false; }

    UINT indices[6] =
    {
        0, 1, 2,
        0, 2, 3
    };
    ZeroMemory(&bdc, sizeof(bdc));
    bdc.Usage = D3D11_USAGE_IMMUTABLE;
    bdc.ByteWidth = sizeof(UINT) * 6;
    bdc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bdc.CPUAccessFlags = 0;
    bdc.StructureByteStride = 0;
    bdc.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData = {};
    ZeroMemory(&iinitData, sizeof(iinitData));
    iinitData.pSysMem = indices;
    hr = Device()->CreateBuffer(
        &bdc, &iinitData, &mIndexBuffer);
    if (FAILED(hr)) { return false; }

    return true;
}

bool RSPass_Ssao::CreateTextures()
{
    HRESULT hr = S_OK;
    std::string name = "";
    DATA_TEXTURE_INFO dti = {};
    D3D11_TEXTURE2D_DESC texDesc = {};
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    D3D11_SUBRESOURCE_DATA iniData = {};
    ZeroMemory(&texDesc, sizeof(texDesc));
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    ZeroMemory(&rtvDesc, sizeof(rtvDesc));
    ZeroMemory(&uavDesc, sizeof(uavDesc));
    ZeroMemory(&iniData, sizeof(iniData));

    ID3D11Texture2D* texture = nullptr;
    ID3D11RenderTargetView* rtv = nullptr;
    ID3D11ShaderResourceView* srv = nullptr;
    ID3D11UnorderedAccessView* uav = nullptr;

    DirectX::PackedVector::XMCOLOR* random = nullptr;
    random = new DirectX::PackedVector::XMCOLOR[256 * 256];
    int basic = 1;
    int range = 100;
    DirectX::XMFLOAT3 v = { 0.f,0.f,0.f };
    std::srand((unsigned int)std::time(nullptr) +
        (unsigned int)std::rand());
    for (int i = 0; i < 256; i++)
    {
        for (int j = 0; j < 256; j++)
        {
            v.x = (float)(std::rand() % range + basic) / 100.f;
            v.y = (float)(std::rand() % range + basic) / 100.f;
            v.z = (float)(std::rand() % range + basic) / 100.f;
            random[i * 256 + j] =
                DirectX::PackedVector::XMCOLOR(v.x, v.y, v.z, 0.f);
        }
    }

    iniData.SysMemPitch = 256 *
        sizeof(DirectX::PackedVector::XMCOLOR);
    iniData.pSysMem = random;

    texDesc.Width = 256;
    texDesc.Height = 256;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    hr = Device()->CreateTexture2D(
        &texDesc, &iniData, &texture);

    delete[] random;
    if (FAILED(hr)) { return false; }

    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    hr = Device()->CreateShaderResourceView(texture,
        &srvDesc, &srv);
    if (FAILED(hr)) { return false; }

    dti = {};
    name = "random-tex-ssao";
    dti.mTexture = texture;
    dti.mSrv = srv;
    g_Root->TexturesManager()->AddDataTexture(name, dti);

    texDesc.Width = 1280;
    texDesc.Height = 720;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.BindFlags =
        D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE |
        D3D11_BIND_UNORDERED_ACCESS;
    hr = Device()->CreateTexture2D(
        &texDesc, nullptr, &texture);
    if (FAILED(hr)) { return false; }

    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvDesc.Texture2D.MipSlice = 0;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    hr = Device()->CreateRenderTargetView(
        texture, &rtvDesc, &rtv);
    if (FAILED(hr)) { return false; }

    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    hr = Device()->CreateShaderResourceView(
        texture, &srvDesc, &srv);
    if (FAILED(hr)) { return false; }

    uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
    uavDesc.Texture2D.MipSlice = 0;
    hr = Device()->CreateUnorderedAccessView(
        texture, &uavDesc, &uav);
    if (FAILED(hr)) { return false; }

    dti = {};
    name = "ssao-tex-ssao";
    dti.mTexture = texture;
    dti.mRtv = rtv;
    dti.mSrv = srv;
    dti.mUav = uav;
    g_Root->TexturesManager()->AddDataTexture(name, dti);

    return true;
}

bool RSPass_Ssao::CreateViews()
{
    std::string name = "random-tex-ssao";
    mRandomMapSrv = g_Root->TexturesManager()->
        GetDataTexInfo(name)->mSrv;
    name = "mrt-normal";
    mNormalMapSrv = g_Root->TexturesManager()->
        GetDataTexInfo(name)->mSrv;
    name = "mrt-depth";
    mDepthMapSrv = g_Root->TexturesManager()->
        GetDataTexInfo(name)->mSrv;
    name = "ssao-tex-ssao";
    mRenderTargetView = g_Root->TexturesManager()->
        GetDataTexInfo(name)->mRtv;

    D3D11_SHADER_RESOURCE_VIEW_DESC desSRV = {};
    HRESULT hr = S_OK;
    ZeroMemory(&desSRV, sizeof(desSRV));
    desSRV.Format = DXGI_FORMAT_UNKNOWN;
    desSRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    desSRV.Buffer.ElementWidth = 1;
    hr = Device()->CreateShaderResourceView(
        mSsaoInfoStructedBuffer,
        &desSRV, &mSsaoInfoStructedBufferSrv);
    if (FAILED(hr)) { return false; }

    return true;
}

bool RSPass_Ssao::CreateSamplers()
{
    HRESULT hr = S_OK;
    D3D11_SAMPLER_DESC samDesc = {};
    ZeroMemory(&samDesc, sizeof(samDesc));

    samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samDesc.MinLOD = 0;
    samDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = Device()->CreateSamplerState(
        &samDesc, &mSamplePointClamp);
    if (FAILED(hr)) { return false; }

    samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samDesc.MinLOD = 0;
    samDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = Device()->CreateSamplerState(
        &samDesc, &mSampleLinearClamp);
    if (FAILED(hr)) { return false; }

    samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    samDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    samDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    samDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    samDesc.MinLOD = 0;
    samDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = Device()->CreateSamplerState(
        &samDesc, &mSampleDepthMap);
    if (FAILED(hr)) { return false; }

    samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samDesc.MinLOD = 0;
    samDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = Device()->CreateSamplerState(
        &samDesc, &mSampleLinearWrap);
    if (FAILED(hr)) { return false; }

    return true;
}

RSPass_KBBlur::RSPass_KBBlur(
    std::string& _name, PASS_TYPE _type, RSRoot_DX11* _root) :
    RSPass_Base(_name, _type, _root),
    mHoriBlurShader(nullptr), mVertBlurShader(nullptr),
    mSsaoTexUav(nullptr),
    mNormalMapSrv(nullptr),
    mDepthMapSrv(nullptr)
{

}

RSPass_KBBlur::RSPass_KBBlur(const RSPass_KBBlur& _source) :
    RSPass_Base(_source),
    mHoriBlurShader(_source.mHoriBlurShader),
    mVertBlurShader(_source.mVertBlurShader),
    mSsaoTexUav(_source.mSsaoTexUav),
    mNormalMapSrv(_source.mNormalMapSrv),
    mDepthMapSrv(_source.mDepthMapSrv)
{

}

RSPass_KBBlur::~RSPass_KBBlur()
{

}

RSPass_KBBlur* RSPass_KBBlur::ClonePass()
{
    return new RSPass_KBBlur(*this);
}

bool RSPass_KBBlur::InitPass()
{
    if (!CreateShaders()) { return false; }
    if (!CreateViews()) { return false; }

    return true;
}

void RSPass_KBBlur::ReleasePass()
{
    RS_RELEASE(mHoriBlurShader);
    RS_RELEASE(mVertBlurShader);
}

void RSPass_KBBlur::ExecuatePass()
{
    ID3D11ShaderResourceView* srv[] =
    {
        mNormalMapSrv, mDepthMapSrv
    };
    static ID3D11UnorderedAccessView* nullUav = nullptr;
    static ID3D11ShaderResourceView* nullSrv[] =
    {
        nullptr, nullptr
    };

    static UINT loopCount = GetRenderConfig().mBlurLoopCount;

    for (UINT i = 0; i < loopCount; i++)
    {
        STContext()->CSSetShader(mHoriBlurShader, nullptr, 0);
        STContext()->CSSetUnorderedAccessViews(0, 1,
            &mSsaoTexUav, nullptr);
        STContext()->CSSetShaderResources(0, 2, srv);
        STContext()->Dispatch(5, 720, 1);
        STContext()->CSSetUnorderedAccessViews(0, 1,
            &nullUav, nullptr);
        STContext()->CSSetShaderResources(0, 2, nullSrv);

        STContext()->CSSetShader(mVertBlurShader, nullptr, 0);
        STContext()->CSSetUnorderedAccessViews(0, 1,
            &mSsaoTexUav, nullptr);
        STContext()->CSSetShaderResources(0, 2, srv);
        STContext()->Dispatch(1280, 3, 1);
        STContext()->CSSetUnorderedAccessViews(0, 1,
            &nullUav, nullptr);
        STContext()->CSSetShaderResources(0, 2, nullSrv);
    }
}

bool RSPass_KBBlur::CreateShaders()
{
    ID3DBlob* shaderBlob = nullptr;
    HRESULT hr = S_OK;

    hr = Tool::CompileShaderFromFile(
        L".\\Shaders\\ssao_compute.hlsl",
        "HMain", "cs_5_0", &shaderBlob);
    if (FAILED(hr)) { return false; }

    hr = Device()->CreateComputeShader(
        shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(),
        nullptr, &mHoriBlurShader);
    shaderBlob->Release();
    shaderBlob = nullptr;
    if (FAILED(hr)) { return false; }

    hr = Tool::CompileShaderFromFile(
        L".\\Shaders\\ssao_compute.hlsl",
        "VMain", "cs_5_0", &shaderBlob);
    if (FAILED(hr)) { return false; }

    hr = Device()->CreateComputeShader(
        shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(),
        nullptr, &mVertBlurShader);
    shaderBlob->Release();
    shaderBlob = nullptr;
    if (FAILED(hr)) { return false; }

    return true;
}

bool RSPass_KBBlur::CreateViews()
{
    std::string name = "mrt-normal";
    mNormalMapSrv = g_Root->TexturesManager()->
        GetDataTexInfo(name)->mSrv;
    name = "mrt-depth";
    mDepthMapSrv = g_Root->TexturesManager()->
        GetDataTexInfo(name)->mSrv;
    name = "ssao-tex-ssao";
    mSsaoTexUav = g_Root->TexturesManager()->
        GetDataTexInfo(name)->mUav;

    return true;
}

RSPass_SkyShpere::RSPass_SkyShpere(std::string& _name,
    PASS_TYPE _type, RSRoot_DX11* _root) :
    RSPass_Base(_name, _type, _root),
    mVertexShader(nullptr), mPixelShader(nullptr),
    mRasterizerState(nullptr), mDepthStencilState(nullptr),
    mRenderTargerView(nullptr), mDepthStencilView(nullptr),
    /*mSkyShpereSrv(nullptr), */mSkyShpereInfoStructedBuffer(nullptr),
    mSkyShpereInfoStructedBufferSrv(nullptr), mSkySphereMesh({}),
    mLinearWrapSampler(nullptr), mRSCameraInfo(nullptr)
{

}

RSPass_SkyShpere::RSPass_SkyShpere(
    const RSPass_SkyShpere& _source) :
    RSPass_Base(_source),
    mVertexShader(_source.mVertexShader),
    mPixelShader(_source.mPixelShader),
    mRasterizerState(_source.mRasterizerState),
    mDepthStencilState(_source.mDepthStencilState),
    mRenderTargerView(_source.mRenderTargerView),
    mDepthStencilView(_source.mDepthStencilView),
    //mSkyShpereSrv(_source.mSkyShpereSrv),
    mSkyShpereInfoStructedBuffer(_source.mSkyShpereInfoStructedBuffer),
    mSkyShpereInfoStructedBufferSrv(_source.mSkyShpereInfoStructedBufferSrv),
    mSkySphereMesh(_source.mSkySphereMesh),
    mLinearWrapSampler(_source.mLinearWrapSampler),
    mRSCameraInfo(_source.mRSCameraInfo)
{

}

RSPass_SkyShpere::~RSPass_SkyShpere()
{

}

RSPass_SkyShpere* RSPass_SkyShpere::ClonePass()
{
    return new RSPass_SkyShpere(*this);
}

bool RSPass_SkyShpere::InitPass()
{
    if (!CreateShaders()) { return false; }
    if (!CreateStates()) { return false; }
    if (!CreateBuffers()) { return false; }
    if (!CreateViews()) { return false; }
    if (!CreateSamplers()) { return false; }

    mSkySphereMesh = g_Root->MeshHelper()->GeoGenerate()->
        CreateGeometrySphere(10.f, 0,
            LAYOUT_TYPE::NORMAL_TANGENT_TEX, false,
            {}, "snow-cube.dds");

    std::string name = "temp-cam";
    mRSCameraInfo = g_Root->CamerasContainer()->
        GetRSCameraInfo(name);

    return true;
}

void RSPass_SkyShpere::ReleasePass()
{
    RS_RELEASE(mVertexShader);
    RS_RELEASE(mPixelShader);
    RS_RELEASE(mRasterizerState);
    RS_RELEASE(mDepthStencilState);
    RS_RELEASE(mLinearWrapSampler);
    RS_RELEASE(mSkyShpereInfoStructedBuffer);
    //RS_RELEASE(mSkyShpereInfoStructedBufferSrv);

    g_Root->MeshHelper()->ReleaseSubMesh(mSkySphereMesh);
}

void RSPass_SkyShpere::ExecuatePass()
{
    ID3D11RenderTargetView* null = nullptr;
    STContext()->OMSetRenderTargets(1,
        &mRenderTargerView, mDepthStencilView);
    STContext()->RSSetViewports(1, &g_ViewPort);
    STContext()->VSSetShader(mVertexShader, nullptr, 0);
    STContext()->PSSetShader(mPixelShader, nullptr, 0);
    STContext()->RSSetState(mRasterizerState);
    STContext()->OMSetDepthStencilState(mDepthStencilState, 0);

    DirectX::XMMATRIX mat = {};
    DirectX::XMFLOAT4X4 flt44 = {};
    UINT stride = sizeof(VERTEX_INFO);
    UINT offset = 0;

    D3D11_MAPPED_SUBRESOURCE msr = {};

    STContext()->Map(mSkyShpereInfoStructedBuffer, 0,
        D3D11_MAP_WRITE_DISCARD, 0, &msr);
    SkyShpereInfo* sp_data = (SkyShpereInfo*)msr.pData;
    mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mViewMat);
    mat = DirectX::XMMatrixTranspose(mat);
    DirectX::XMStoreFloat4x4(&sp_data[0].mViewMat, mat);

    mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mProjMat);
    mat = DirectX::XMMatrixTranspose(mat);
    DirectX::XMStoreFloat4x4(&sp_data[0].mProjMat, mat);

    sp_data[0].mEyePosition = mRSCameraInfo->mEyePosition;

    mat = DirectX::XMMatrixScaling(1000.f, 1000.f, 1000.f);
    mat = DirectX::XMMatrixTranspose(mat);
    DirectX::XMStoreFloat4x4(&sp_data[0].mWorldMat, mat);
    STContext()->Unmap(mSkyShpereInfoStructedBuffer, 0);

    STContext()->IASetInputLayout(mSkySphereMesh.mLayout);
    STContext()->IASetPrimitiveTopology(
        mSkySphereMesh.mTopologyType);
    STContext()->IASetVertexBuffers(
        0, 1, &mSkySphereMesh.mVertexBuffer,
        &stride, &offset);
    STContext()->IASetIndexBuffer(
        mSkySphereMesh.mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    STContext()->VSSetShaderResources(
        0, 1, &mSkyShpereInfoStructedBufferSrv);
    static std::string tex = mSkySphereMesh.mTextures[0];
    static ID3D11ShaderResourceView* cube = nullptr;
    cube = g_Root->TexturesManager()->GetMeshSrv(tex);
    STContext()->PSSetShaderResources(
        0, 1, &cube);
    STContext()->PSSetSamplers(0, 1, &mLinearWrapSampler);

    STContext()->DrawIndexedInstanced(mSkySphereMesh.mIndexCount,
        1, 0, 0, 0);

    STContext()->OMSetRenderTargets(1, &null, nullptr);
    STContext()->RSSetState(nullptr);
    STContext()->OMSetDepthStencilState(nullptr, 0);
}

bool RSPass_SkyShpere::CreateShaders()
{
    ID3DBlob* shaderBlob = nullptr;
    HRESULT hr = S_OK;

    hr = Tool::CompileShaderFromFile(
        L".\\Shaders\\skysphere_vertex.hlsl",
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
        L".\\Shaders\\skysphere_pixel.hlsl",
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

bool RSPass_SkyShpere::CreateStates()
{
    HRESULT hr = S_OK;
    D3D11_RASTERIZER_DESC rasDesc = {};
    D3D11_DEPTH_STENCIL_DESC depDesc = {};
    ZeroMemory(&rasDesc, sizeof(rasDesc));
    ZeroMemory(&depDesc, sizeof(depDesc));

    rasDesc.CullMode = D3D11_CULL_NONE;
    rasDesc.FillMode = D3D11_FILL_SOLID;
    depDesc.DepthEnable = TRUE;
    depDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

    hr = Device()->CreateRasterizerState(
        &rasDesc, &mRasterizerState);
    if (FAILED(hr)) { return false; }

    hr = Device()->CreateDepthStencilState(
        &depDesc, &mDepthStencilState);
    if (FAILED(hr)) { return false; }

    return true;
}

bool RSPass_SkyShpere::CreateBuffers()
{
    HRESULT hr = S_OK;
    D3D11_BUFFER_DESC bdc = {};

    ZeroMemory(&bdc, sizeof(bdc));
    bdc.Usage = D3D11_USAGE_DYNAMIC;
    bdc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bdc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    bdc.ByteWidth = sizeof(SkyShpereInfo);
    bdc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bdc.StructureByteStride = sizeof(SkyShpereInfo);
    hr = Device()->CreateBuffer(
        &bdc, nullptr, &mSkyShpereInfoStructedBuffer);
    if (FAILED(hr)) { return false; }

    return true;
}

bool RSPass_SkyShpere::CreateViews()
{
    mRenderTargerView = g_Root->Devices()->GetSwapChainRtv();
    std::string name = "mrt-depth";
    mDepthStencilView = g_Root->TexturesManager()->
        GetDataTexInfo(name)->mDsv;

    D3D11_SHADER_RESOURCE_VIEW_DESC desSRV = {};
    HRESULT hr = S_OK;
    ZeroMemory(&desSRV, sizeof(desSRV));
    desSRV.Format = DXGI_FORMAT_UNKNOWN;
    desSRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    desSRV.Buffer.ElementWidth = 1;
    hr = Device()->CreateShaderResourceView(
        mSkyShpereInfoStructedBuffer,
        &desSRV, &mSkyShpereInfoStructedBufferSrv);
    if (FAILED(hr)) { return false; }

    return true;
}

bool RSPass_SkyShpere::CreateSamplers()
{
    HRESULT hr = S_OK;
    D3D11_SAMPLER_DESC samDesc = {};
    ZeroMemory(&samDesc, sizeof(samDesc));

    samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samDesc.MinLOD = 0;
    samDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = Device()->CreateSamplerState(
        &samDesc, &mLinearWrapSampler);
    if (FAILED(hr)) { return false; }

    return true;
}

RSPass_Sprite::RSPass_Sprite(std::string& _name,
    PASS_TYPE _type, RSRoot_DX11* _root) :
    RSPass_Base(_name, _type, _root),
    mVertexShader(nullptr), mPixelShader(nullptr),
    mDepthStencilState(nullptr), mRenderTargetView(nullptr),
    mProjStructedBuffer(nullptr), mProjStructedBufferSrv(nullptr),
    mInstanceStructedBuffer(nullptr),
    mInstanceStructedBufferSrv(nullptr),
    mLinearSampler(nullptr), mBlendState(nullptr),
    mDrawCallType(DRAWCALL_TYPE::MAX), mDrawCallPipe(nullptr),
    mRSCameraInfo(nullptr)
{

}

RSPass_Sprite::RSPass_Sprite(
    const RSPass_Sprite& _source) :
    RSPass_Base(_source),
    mVertexShader(_source.mVertexShader),
    mPixelShader(_source.mPixelShader),
    mDepthStencilState(_source.mDepthStencilState),
    mBlendState(_source.mBlendState),
    mRenderTargetView(_source.mRenderTargetView),
    mDrawCallType(_source.mDrawCallType),
    mDrawCallPipe(_source.mDrawCallPipe),
    mProjStructedBuffer(_source.mProjStructedBuffer),
    mProjStructedBufferSrv(_source.mProjStructedBufferSrv),
    mInstanceStructedBuffer(_source.mInstanceStructedBuffer),
    mInstanceStructedBufferSrv(_source.mInstanceStructedBufferSrv),
    mLinearSampler(_source.mLinearSampler),
    mRSCameraInfo(_source.mRSCameraInfo)
{

}

RSPass_Sprite::~RSPass_Sprite()
{

}

RSPass_Sprite* RSPass_Sprite::ClonePass()
{
    return new RSPass_Sprite(*this);
}

bool RSPass_Sprite::InitPass()
{
    if (!CreateShaders()) { return false; }
    if (!CreateStates()) { return false; }
    if (!CreateBuffers()) { return false; }
    if (!CreateViews()) { return false; }
    if (!CreateSamplers()) { return false; }

    mDrawCallType = DRAWCALL_TYPE::UI_SPRITE;
    mDrawCallPipe = g_Root->DrawCallsPool()->
        GetDrawCallsPipe(mDrawCallType);

    std::string name = "temp-ui-cam";
    mRSCameraInfo = g_Root->CamerasContainer()->
        GetRSCameraInfo(name);

    return true;
}

void RSPass_Sprite::ReleasePass()
{
    RS_RELEASE(mVertexShader);
    RS_RELEASE(mPixelShader);
    RS_RELEASE(mDepthStencilState);
    RS_RELEASE(mLinearSampler);
    RS_RELEASE(mProjStructedBufferSrv);
    RS_RELEASE(mProjStructedBuffer);
    RS_RELEASE(mInstanceStructedBufferSrv);
    RS_RELEASE(mInstanceStructedBuffer);
}

void RSPass_Sprite::ExecuatePass()
{
    ID3D11RenderTargetView* rtvnull = nullptr;
    STContext()->OMSetRenderTargets(1,
        &mRenderTargetView, nullptr);
    STContext()->RSSetViewports(1, &g_ViewPort);
    STContext()->OMSetDepthStencilState(mDepthStencilState, 0);
    static float factor[4] = { 0.f,0.f,0.f,0.f };
    STContext()->OMSetBlendState(mBlendState, factor, 0xFFFFFFFF);
    STContext()->VSSetShader(mVertexShader, nullptr, 0);
    STContext()->PSSetShader(mPixelShader, nullptr, 0);
    STContext()->PSSetSamplers(0, 1, &mLinearSampler);

    DirectX::XMMATRIX mat = {};
    DirectX::XMFLOAT4X4 flt44 = {};
    UINT stride = sizeof(VERTEX_INFO);
    UINT offset = 0;

    D3D11_MAPPED_SUBRESOURCE msr = {};
    STContext()->Map(mProjStructedBuffer, 0,
        D3D11_MAP_WRITE_DISCARD, 0, &msr);
    OnlyProj* vp_data = (OnlyProj*)msr.pData;
    mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mProjMat);
    mat = DirectX::XMMatrixTranspose(mat);
    DirectX::XMStoreFloat4x4(&vp_data[0].mProjMat, mat);
    STContext()->Unmap(mProjStructedBuffer, 0);

    STContext()->VSSetShaderResources(
        0, 1, &mProjStructedBufferSrv);

    for (auto& call : mDrawCallPipe->mDatas)
    {
        auto vecPtr = call.mInstanceData.mDataPtr;
        auto size = vecPtr->size();
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
            1, 1, &mInstanceStructedBufferSrv);
        STContext()->PSSetShaderResources(
            0, 1, &(call.mTextureDatas[0].mSrv));

        STContext()->DrawIndexedInstanced(
            call.mMeshData.mIndexCount,
            (UINT)call.mInstanceData.mDataPtr->size(), 0, 0, 0);
    }

    STContext()->OMSetRenderTargets(1, &rtvnull, nullptr);
    STContext()->OMSetDepthStencilState(nullptr, 0);
    STContext()->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
}

bool RSPass_Sprite::CreateShaders()
{
    ID3DBlob* shaderBlob = nullptr;
    HRESULT hr = S_OK;

    hr = Tool::CompileShaderFromFile(
        L".\\Shaders\\sprite_vertex.hlsl",
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
        L".\\Shaders\\sprite_pixel.hlsl",
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

bool RSPass_Sprite::CreateStates()
{
    HRESULT hr = S_OK;

    D3D11_DEPTH_STENCIL_DESC depDesc = {};
    depDesc.DepthEnable = FALSE;
    depDesc.StencilEnable = FALSE;
    hr = Device()->CreateDepthStencilState(
        &depDesc, &mDepthStencilState);
    if (FAILED(hr)) { return false; }

    D3D11_BLEND_DESC bldDesc = {};
    bldDesc.AlphaToCoverageEnable = FALSE;
    bldDesc.IndependentBlendEnable = FALSE;
    bldDesc.RenderTarget[0].BlendEnable = TRUE;
    bldDesc.RenderTarget[0].RenderTargetWriteMask =
        D3D11_COLOR_WRITE_ENABLE_ALL;
    bldDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    bldDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    bldDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    bldDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    bldDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    bldDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    hr = Device()->CreateBlendState(&bldDesc, &mBlendState);
    if (FAILED(hr)) { return false; }

    return true;
}

bool RSPass_Sprite::CreateBuffers()
{
    HRESULT hr = S_OK;
    D3D11_BUFFER_DESC bdc = {};

    ZeroMemory(&bdc, sizeof(bdc));
    bdc.Usage = D3D11_USAGE_DYNAMIC;
    bdc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bdc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    bdc.ByteWidth = MAX_INSTANCE_SIZE * sizeof(RS_INSTANCE_DATA);
    bdc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bdc.StructureByteStride = sizeof(RS_INSTANCE_DATA);
    hr = Device()->CreateBuffer(
        &bdc, nullptr, &mInstanceStructedBuffer);
    if (FAILED(hr)) { return false; }

    bdc.ByteWidth = sizeof(OnlyProj);
    bdc.StructureByteStride = sizeof(OnlyProj);
    hr = Device()->CreateBuffer(
        &bdc, nullptr, &mProjStructedBuffer);
    if (FAILED(hr)) { return false; }

    return true;
}

bool RSPass_Sprite::CreateViews()
{
    mRenderTargetView = g_Root->Devices()->GetSwapChainRtv();

    D3D11_SHADER_RESOURCE_VIEW_DESC desSRV = {};
    HRESULT hr = S_OK;
    ZeroMemory(&desSRV, sizeof(desSRV));
    desSRV.Format = DXGI_FORMAT_UNKNOWN;
    desSRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    desSRV.Buffer.ElementWidth = MAX_INSTANCE_SIZE;
    hr = Device()->CreateShaderResourceView(
        mInstanceStructedBuffer,
        &desSRV, &mInstanceStructedBufferSrv);
    if (FAILED(hr)) { return false; }

    desSRV.Buffer.ElementWidth = 1;
    hr = Device()->CreateShaderResourceView(
        mProjStructedBuffer,
        &desSRV, &mProjStructedBufferSrv);
    if (FAILED(hr)) { return false; }

    return true;
}

bool RSPass_Sprite::CreateSamplers()
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
        &sampDesc, &mLinearSampler);
    if (FAILED(hr)) { return false; }

    return true;
}

RSPass_MRT::RSPass_MRT(std::string& _name, PASS_TYPE _type,
    RSRoot_DX11* _root) :
    RSPass_Base(_name, _type, _root),
    mDrawCallType(DRAWCALL_TYPE::OPACITY), mDrawCallPipe(nullptr),
    mVertexShader(nullptr), mPixelShader(nullptr),
    mNDPixelShader(nullptr),
    mViewProjStructedBuffer(nullptr),
    mViewProjStructedBufferSrv(nullptr),
    mInstanceStructedBuffer(nullptr),
    mInstanceStructedBufferSrv(nullptr),
    mLinearSampler(nullptr), mDepthDsv(nullptr),
    mDiffuseRtv(nullptr), mNormalRtv(nullptr),
    mRSCameraInfo(nullptr), mWorldPosRtv(nullptr),
    mDiffAlbeRtv(nullptr), mFresShinRtv(nullptr)
{

}

RSPass_MRT::RSPass_MRT(const RSPass_MRT& _source) :
    RSPass_Base(_source),
    mDrawCallType(_source.mDrawCallType),
    mDrawCallPipe(_source.mDrawCallPipe),
    mVertexShader(_source.mVertexShader),
    mPixelShader(_source.mPixelShader),
    mNDPixelShader(_source.mNDPixelShader),
    mViewProjStructedBuffer(_source.mViewProjStructedBuffer),
    mInstanceStructedBuffer(_source.mInstanceStructedBuffer),
    mViewProjStructedBufferSrv(_source.mViewProjStructedBufferSrv),
    mInstanceStructedBufferSrv(_source.mInstanceStructedBufferSrv),
    mLinearSampler(_source.mLinearSampler),
    mDiffuseRtv(_source.mDiffuseRtv),
    mNormalRtv(_source.mNormalRtv),
    mDepthDsv(_source.mDepthDsv),
    mRSCameraInfo(_source.mRSCameraInfo),
    mWorldPosRtv(_source.mWorldPosRtv),
    mDiffAlbeRtv(_source.mDiffAlbeRtv),
    mFresShinRtv(_source.mFresShinRtv)
{

}

RSPass_MRT::~RSPass_MRT()
{

}

RSPass_MRT* RSPass_MRT::ClonePass()
{
    return new RSPass_MRT(*this);
}

bool RSPass_MRT::InitPass()
{
    if (!CreateShaders()) { return false; }
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

void RSPass_MRT::ReleasePass()
{
    RS_RELEASE(mVertexShader);
    RS_RELEASE(mPixelShader);
    RS_RELEASE(mViewProjStructedBuffer);
    RS_RELEASE(mViewProjStructedBufferSrv);
    RS_RELEASE(mInstanceStructedBuffer);
    RS_RELEASE(mInstanceStructedBufferSrv);
    RS_RELEASE(mLinearSampler);

    std::string name = "mrt-depth";
    g_Root->TexturesManager()->DeleteDataTex(name);
    name = "mrt-normal";
    g_Root->TexturesManager()->DeleteDataTex(name);
    name = "mrt-diffuse";
    g_Root->TexturesManager()->DeleteDataTex(name);
    name = "mrt-worldpos";
    g_Root->TexturesManager()->DeleteDataTex(name);
    name = "mrt-diffuse-albedo";
    g_Root->TexturesManager()->DeleteDataTex(name);
    name = "mrt-fresnel-shinese";
    g_Root->TexturesManager()->DeleteDataTex(name);
}

void RSPass_MRT::ExecuatePass()
{
    ID3D11RenderTargetView* rtvnull = nullptr;
    static ID3D11RenderTargetView* mrt[] = { mDiffuseRtv,
        mNormalRtv,mWorldPosRtv,mDiffAlbeRtv,mFresShinRtv };
    static ID3D11RenderTargetView* nd_mrt[] = { mNormalRtv };
    if (g_RenderInDeferred)
    {
        STContext()->OMSetRenderTargets(5,
            mrt, mDepthDsv);
        STContext()->RSSetViewports(1, &g_ViewPort);
        STContext()->ClearRenderTargetView(
            mDiffuseRtv, DirectX::Colors::DarkGreen);
        STContext()->ClearRenderTargetView(
            mNormalRtv, DirectX::Colors::Transparent);
        STContext()->ClearRenderTargetView(
            mWorldPosRtv, DirectX::Colors::Transparent);
        STContext()->ClearRenderTargetView(
            mDiffAlbeRtv, DirectX::Colors::Transparent);
        STContext()->ClearRenderTargetView(
            mFresShinRtv, DirectX::Colors::Transparent);
        STContext()->ClearDepthStencilView(
            mDepthDsv, D3D11_CLEAR_DEPTH, 1.f, 0);
        STContext()->VSSetShader(mVertexShader, nullptr, 0);
        STContext()->PSSetShader(mPixelShader, nullptr, 0);
    }
    else
    {
        STContext()->OMSetRenderTargets(1,
            nd_mrt, mDepthDsv);
        STContext()->RSSetViewports(1, &g_ViewPort);
        STContext()->ClearRenderTargetView(
            mNormalRtv, DirectX::Colors::Transparent);
        STContext()->ClearDepthStencilView(
            mDepthDsv, D3D11_CLEAR_DEPTH, 1.f, 0);
        STContext()->VSSetShader(mVertexShader, nullptr, 0);
        STContext()->PSSetShader(mNDPixelShader, nullptr, 0);
    }

    STContext()->PSSetSamplers(0, 1, &mLinearSampler);

    DirectX::XMMATRIX mat = {};
    DirectX::XMFLOAT4X4 flt44 = {};
    UINT stride = sizeof(VERTEX_INFO);
    UINT offset = 0;

    D3D11_MAPPED_SUBRESOURCE msr = {};
    STContext()->Map(mViewProjStructedBuffer, 0,
        D3D11_MAP_WRITE_DISCARD, 0, &msr);
    ViewProj* vp_data = (ViewProj*)msr.pData;
    mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mViewMat);
    mat = DirectX::XMMatrixTranspose(mat);
    DirectX::XMStoreFloat4x4(&vp_data[0].mViewMat, mat);
    mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mProjMat);
    mat = DirectX::XMMatrixTranspose(mat);
    DirectX::XMStoreFloat4x4(&vp_data[0].mProjMat, mat);
    STContext()->Unmap(mViewProjStructedBuffer, 0);

    STContext()->VSSetShaderResources(
        0, 1, &mViewProjStructedBufferSrv);

    for (auto& call : mDrawCallPipe->mDatas)
    {
        auto vecPtr = call.mInstanceData.mDataPtr;
        auto size = vecPtr->size();
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
            1, 1, &mInstanceStructedBufferSrv);
        STContext()->PSSetShaderResources(
            0, 1, &(call.mTextureDatas[0].mSrv));
        if (call.mTextureDatas[1].mUse)
        {
            STContext()->PSSetShaderResources(
                1, 1, &(call.mTextureDatas[1].mSrv));
        }

        STContext()->DrawIndexedInstanced(
            call.mMeshData.mIndexCount,
            (UINT)call.mInstanceData.mDataPtr->size(), 0, 0, 0);
    }

    STContext()->OMSetRenderTargets(1, &rtvnull, nullptr);
}

bool RSPass_MRT::CreateShaders()
{
    ID3DBlob* shaderBlob = nullptr;
    HRESULT hr = S_OK;

    hr = Tool::CompileShaderFromFile(
        L".\\Shaders\\mrt_vertex.hlsl",
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
        L".\\Shaders\\mrt_pixel.hlsl",
        "main", "ps_5_0", &shaderBlob);
    if (FAILED(hr)) { return false; }

    hr = Device()->CreatePixelShader(
        shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(),
        nullptr, &mPixelShader);
    shaderBlob->Release();
    shaderBlob = nullptr;
    if (FAILED(hr)) { return false; }

    hr = Tool::CompileShaderFromFile(
        L".\\Shaders\\mrt_nd_pixel.hlsl",
        "main", "ps_5_0", &shaderBlob);
    if (FAILED(hr)) { return false; }

    hr = Device()->CreatePixelShader(
        shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(),
        nullptr, &mNDPixelShader);
    shaderBlob->Release();
    shaderBlob = nullptr;
    if (FAILED(hr)) { return false; }

    return true;
}

bool RSPass_MRT::CreateBuffers()
{
    HRESULT hr = S_OK;
    D3D11_BUFFER_DESC bdc = {};

    ZeroMemory(&bdc, sizeof(bdc));
    bdc.Usage = D3D11_USAGE_DYNAMIC;
    bdc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bdc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    bdc.ByteWidth = MAX_INSTANCE_SIZE * sizeof(RS_INSTANCE_DATA);
    bdc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bdc.StructureByteStride = sizeof(RS_INSTANCE_DATA);
    hr = Device()->CreateBuffer(
        &bdc, nullptr, &mInstanceStructedBuffer);
    if (FAILED(hr)) { return false; }

    bdc.ByteWidth = sizeof(ViewProj);
    bdc.StructureByteStride = sizeof(ViewProj);
    hr = Device()->CreateBuffer(
        &bdc, nullptr, &mViewProjStructedBuffer);
    if (FAILED(hr)) { return false; }

    return true;
}

bool RSPass_MRT::CreateViews()
{
    HRESULT hr = S_OK;
    D3D11_TEXTURE2D_DESC texDesc = {};
    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    ID3D11ShaderResourceView* srv = nullptr;

    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.ElementWidth = MAX_INSTANCE_SIZE;
    hr = Device()->CreateShaderResourceView(
        mInstanceStructedBuffer,
        &srvDesc, &mInstanceStructedBufferSrv);
    if (FAILED(hr)) { return false; }

    srvDesc.Buffer.ElementWidth = 1;
    hr = Device()->CreateShaderResourceView(
        mViewProjStructedBuffer,
        &srvDesc, &mViewProjStructedBufferSrv);
    if (FAILED(hr)) { return false; }

    ZeroMemory(&texDesc, sizeof(texDesc));
    ZeroMemory(&rtvDesc, sizeof(rtvDesc));
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    ZeroMemory(&dsvDesc, sizeof(dsvDesc));
    DATA_TEXTURE_INFO dti = {};
    std::string name = "";

    ID3D11Texture2D* texture = nullptr;
    texDesc.Width = 1280;
    texDesc.Height = 720;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL |
        D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;
    hr = Device()->CreateTexture2D(
        &texDesc, nullptr, &texture);
    if (FAILED(hr)) { return false; }

    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;
    hr = Device()->CreateDepthStencilView(
        texture, &dsvDesc, &mDepthDsv);
    if (FAILED(hr)) { return false; }

    srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    hr = Device()->CreateShaderResourceView(
        texture, &srvDesc, &srv);
    if (FAILED(hr)) { return false; }

    dti = {};
    name = "mrt-depth";
    dti.mTexture = texture;
    dti.mDsv = mDepthDsv;
    dti.mSrv = srv;
    g_Root->TexturesManager()->AddDataTexture(name, dti);

    texDesc.Width = 1280;
    texDesc.Height = 720;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;
    texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    texDesc.BindFlags =
        D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    hr = Device()->CreateTexture2D(
        &texDesc, nullptr, &texture);
    if (FAILED(hr)) { return false; }

    rtvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    rtvDesc.Texture2D.MipSlice = 0;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    hr = Device()->CreateRenderTargetView(
        texture, &rtvDesc, &mNormalRtv);
    if (FAILED(hr)) { return false; }

    srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    hr = Device()->CreateShaderResourceView(texture,
        &srvDesc, &srv);
    if (FAILED(hr)) { return false; }

    dti = {};
    name = "mrt-normal";
    dti.mTexture = texture;
    dti.mRtv = mNormalRtv;
    dti.mSrv = srv;
    g_Root->TexturesManager()->AddDataTexture(name, dti);

    texDesc.Width = 1280;
    texDesc.Height = 720;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;
    texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    texDesc.BindFlags =
        D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    hr = Device()->CreateTexture2D(
        &texDesc, nullptr, &texture);
    if (FAILED(hr)) { return false; }

    rtvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    rtvDesc.Texture2D.MipSlice = 0;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    hr = Device()->CreateRenderTargetView(
        texture, &rtvDesc, &mWorldPosRtv);
    if (FAILED(hr)) { return false; }

    srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    hr = Device()->CreateShaderResourceView(texture,
        &srvDesc, &srv);
    if (FAILED(hr)) { return false; }

    dti = {};
    name = "mrt-worldpos";
    dti.mTexture = texture;
    dti.mRtv = mWorldPosRtv;
    dti.mSrv = srv;
    g_Root->TexturesManager()->AddDataTexture(name, dti);

    texDesc.Width = 1280;
    texDesc.Height = 720;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.BindFlags =
        D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    hr = Device()->CreateTexture2D(
        &texDesc, nullptr, &texture);
    if (FAILED(hr)) { return false; }

    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvDesc.Texture2D.MipSlice = 0;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    hr = Device()->CreateRenderTargetView(
        texture, &rtvDesc, &mDiffuseRtv);
    if (FAILED(hr)) { return false; }

    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    hr = Device()->CreateShaderResourceView(texture,
        &srvDesc, &srv);
    if (FAILED(hr)) { return false; }

    dti = {};
    name = "mrt-diffuse";
    dti.mTexture = texture;
    dti.mRtv = mDiffuseRtv;
    dti.mSrv = srv;
    g_Root->TexturesManager()->AddDataTexture(name, dti);

    texDesc.Width = 1280;
    texDesc.Height = 720;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.BindFlags =
        D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    hr = Device()->CreateTexture2D(
        &texDesc, nullptr, &texture);
    if (FAILED(hr)) { return false; }

    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvDesc.Texture2D.MipSlice = 0;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    hr = Device()->CreateRenderTargetView(
        texture, &rtvDesc, &mDiffAlbeRtv);
    if (FAILED(hr)) { return false; }

    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    hr = Device()->CreateShaderResourceView(texture,
        &srvDesc, &srv);
    if (FAILED(hr)) { return false; }

    dti = {};
    name = "mrt-diffuse-albedo";
    dti.mTexture = texture;
    dti.mRtv = mDiffAlbeRtv;
    dti.mSrv = srv;
    g_Root->TexturesManager()->AddDataTexture(name, dti);

    texDesc.Width = 1280;
    texDesc.Height = 720;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.BindFlags =
        D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    hr = Device()->CreateTexture2D(
        &texDesc, nullptr, &texture);
    if (FAILED(hr)) { return false; }

    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvDesc.Texture2D.MipSlice = 0;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    hr = Device()->CreateRenderTargetView(
        texture, &rtvDesc, &mFresShinRtv);
    if (FAILED(hr)) { return false; }

    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    hr = Device()->CreateShaderResourceView(texture,
        &srvDesc, &srv);
    if (FAILED(hr)) { return false; }

    dti = {};
    name = "mrt-fresnel-shinese";
    dti.mTexture = texture;
    dti.mRtv = mFresShinRtv;
    dti.mSrv = srv;
    g_Root->TexturesManager()->AddDataTexture(name, dti);

    return true;
}

bool RSPass_MRT::CreateSamplers()
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
        &sampDesc, &mLinearSampler);
    if (FAILED(hr)) { return false; }

    return true;
}

RSPass_Defered::RSPass_Defered(
    std::string& _name, PASS_TYPE _type, RSRoot_DX11* _root) :
    RSPass_Base(_name, _type, _root),
    mVertexShader(nullptr), mPixelShader(nullptr),
    mLinearWrapSampler(nullptr), mRenderTargetView(nullptr),
    mShadowTexSampler(nullptr), mPointClampSampler(nullptr),
    mLightStructedBuffer(nullptr),
    mLightStructedBufferSrv(nullptr),
    mLightInfoStructedBuffer(nullptr),
    mLightInfoStructedBufferSrv(nullptr),
    mAmbientStructedBuffer(nullptr),
    mAmbientStructedBufferSrv(nullptr),
    mShadowStructedBuffer(nullptr),
    mShadowStructedBufferSrv(nullptr),
    mSsaoSrv(nullptr),
    mVertexBuffer(nullptr), mIndexBuffer(nullptr),
    mWorldPosSrv(nullptr), mNormalSrv(nullptr), mDiffuseSrv(nullptr),
    mDiffuseAlbedoSrv(nullptr), mFresenlShineseSrv(nullptr),
    mRSCameraInfo(nullptr), mShadowDepthSrv(nullptr)
{

}

RSPass_Defered::RSPass_Defered(const RSPass_Defered& _source) :
    RSPass_Base(_source),
    mVertexShader(_source.mVertexShader),
    mPixelShader(_source.mPixelShader),
    mRenderTargetView(_source.mRenderTargetView),
    mLinearWrapSampler(_source.mLinearWrapSampler),
    mPointClampSampler(_source.mPointClampSampler),
    mShadowTexSampler(_source.mShadowTexSampler),
    mLightStructedBuffer(_source.mLightStructedBuffer),
    mLightStructedBufferSrv(_source.mLightStructedBufferSrv),
    mLightInfoStructedBuffer(_source.mLightInfoStructedBuffer),
    mLightInfoStructedBufferSrv(_source.mLightInfoStructedBufferSrv),
    mAmbientStructedBuffer(_source.mAmbientStructedBuffer),
    mAmbientStructedBufferSrv(_source.mAmbientStructedBufferSrv),
    mShadowStructedBuffer(_source.mShadowStructedBuffer),
    mShadowStructedBufferSrv(_source.mShadowStructedBufferSrv),
    mSsaoSrv(_source.mSsaoSrv),
    mVertexBuffer(_source.mVertexBuffer),
    mIndexBuffer(_source.mIndexBuffer),
    mWorldPosSrv(_source.mWorldPosSrv),
    mNormalSrv(_source.mNormalSrv),
    mDiffuseSrv(_source.mDiffuseSrv),
    mDiffuseAlbedoSrv(_source.mDiffuseAlbedoSrv),
    mFresenlShineseSrv(_source.mFresenlShineseSrv),
    mRSCameraInfo(_source.mRSCameraInfo),
    mShadowDepthSrv(_source.mShadowDepthSrv)
{

}

RSPass_Defered::~RSPass_Defered()
{

}

RSPass_Defered* RSPass_Defered::ClonePass()
{
    return new RSPass_Defered(*this);
}

bool RSPass_Defered::InitPass()
{
    if (!CreateShaders()) { return false; }
    if (!CreateBuffers()) { return false; }
    if (!CreateViews()) { return false; }
    if (!CreateSamplers()) { return false; }

    std::string name = "temp-cam";
    mRSCameraInfo = g_Root->CamerasContainer()->
        GetRSCameraInfo(name);

    return true;
}

void RSPass_Defered::ReleasePass()
{
    RS_RELEASE(mVertexShader);
    RS_RELEASE(mPixelShader);
    RS_RELEASE(mLinearWrapSampler);
    RS_RELEASE(mPointClampSampler);
    RS_RELEASE(mShadowTexSampler);
    RS_RELEASE(mLightInfoStructedBuffer);
    RS_RELEASE(mLightInfoStructedBufferSrv);
    RS_RELEASE(mLightStructedBuffer);
    RS_RELEASE(mLightStructedBufferSrv);
    RS_RELEASE(mAmbientStructedBuffer);
    RS_RELEASE(mAmbientStructedBufferSrv);
    RS_RELEASE(mShadowStructedBuffer);
    RS_RELEASE(mShadowStructedBufferSrv);
    RS_RELEASE(mVertexBuffer);
    RS_RELEASE(mIndexBuffer);
}

void RSPass_Defered::ExecuatePass()
{
    STContext()->OMSetRenderTargets(1,
        &mRenderTargetView, nullptr);
    STContext()->RSSetViewports(1, &g_ViewPort);
    STContext()->ClearRenderTargetView(
        mRenderTargetView, DirectX::Colors::DarkGreen);
    STContext()->VSSetShader(mVertexShader, nullptr, 0);
    STContext()->PSSetShader(mPixelShader, nullptr, 0);

    DirectX::XMMATRIX mat = {};
    DirectX::XMFLOAT4X4 flt44 = {};
    UINT stride = sizeof(VERTEX_INFO);
    UINT offset = 0;

    D3D11_MAPPED_SUBRESOURCE msr = {};
    STContext()->Map(mAmbientStructedBuffer, 0,
        D3D11_MAP_WRITE_DISCARD, 0, &msr);
    Ambient* amb_data = (Ambient*)msr.pData;
    static DirectX::XMFLOAT4 ambientL =
        GetRSRoot_DX11_Singleton()->LightsContainer()->
        GetCurrentAmbientLight();
    amb_data[0].mAmbient = ambientL;
    STContext()->Unmap(mAmbientStructedBuffer, 0);

    static auto lights = g_Root->LightsContainer()->GetLights();
    STContext()->Map(mLightInfoStructedBuffer, 0,
        D3D11_MAP_WRITE_DISCARD, 0, &msr);
    LightInfo* li_data = (LightInfo*)msr.pData;
    li_data[0].mCameraPos = mRSCameraInfo->mEyePosition;
    UINT dNum = 0;
    UINT sNum = 0;
    UINT pNum = 0;
    for (auto& l : *lights)
    {
        auto type = l->GetRSLightType();
        switch (type)
        {
        case LIGHT_TYPE::DIRECT:
            ++dNum; break;
        case LIGHT_TYPE::POINT:
            ++pNum; break;
        case LIGHT_TYPE::SPOT:
            ++sNum; break;
        default: break;
        }
    }
    li_data[0].mDirectLightNum = dNum;
    li_data[0].mPointLightNum = pNum;
    li_data[0].mSpotLightNum = sNum;
    li_data[0].mShadowLightNum = (UINT)g_Root->LightsContainer()->
        GetShadowLights()->size();
    li_data[0].mShadowLightIndex[0] = -1;
    li_data[0].mShadowLightIndex[1] = -1;
    li_data[0].mShadowLightIndex[2] = -1;
    li_data[0].mShadowLightIndex[3] = -1;
    auto shadowIndeices = g_Root->LightsContainer()->
        GetShadowLightIndeices();
    for (UINT i = 0; i < li_data[0].mShadowLightNum; i++)
    {
        li_data[0].mShadowLightIndex[i] = (*shadowIndeices)[i];
        if (i >= 3)
        {
            break;
        }
    }
    STContext()->Unmap(mLightInfoStructedBuffer, 0);

    STContext()->Map(mLightStructedBuffer, 0,
        D3D11_MAP_WRITE_DISCARD, 0, &msr);
    RS_LIGHT_INFO* l_data = (RS_LIGHT_INFO*)msr.pData;
    UINT lightIndex = 0;
    for (auto& l : *lights)
    {
        l_data[lightIndex++] = *(l->GetRSLightInfo());
    }
    STContext()->Unmap(mLightStructedBuffer, 0);

    STContext()->Map(mShadowStructedBuffer, 0,
        D3D11_MAP_WRITE_DISCARD, 0, &msr);
    ShadowInfo* s_data = (ShadowInfo*)msr.pData;
    auto shadowLights = g_Root->LightsContainer()->
        GetShadowLights();
    UINT shadowSize = (UINT)shadowLights->size();
    for (UINT i = 0; i < shadowSize; i++)
    {
        auto lcam = (*shadowLights)[i]->GetRSLightCamera();
        mat = DirectX::XMLoadFloat4x4(
            &(lcam->GetRSCameraInfo()->mViewMat));
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&s_data[i].mShadowViewMat, mat);
        mat = DirectX::XMLoadFloat4x4(
            &(lcam->GetRSCameraInfo()->mProjMat));
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&s_data[i].mShadowProjMat, mat);
    }

    static DirectX::XMMATRIX T(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f);
    mat = DirectX::XMMatrixTranspose(
        DirectX::XMLoadFloat4x4(&mRSCameraInfo->mViewMat) *
        DirectX::XMLoadFloat4x4(&mRSCameraInfo->mProjMat) *
        T);
    DirectX::XMStoreFloat4x4(&s_data[0].mSSAOMat, mat);
    STContext()->Unmap(mShadowStructedBuffer, 0);

    static ID3D11ShaderResourceView* srvs[] =
    {
        mAmbientStructedBufferSrv,
        mLightInfoStructedBufferSrv,
        mLightStructedBufferSrv,
        mShadowStructedBufferSrv,
        mWorldPosSrv, mNormalSrv, mDiffuseSrv,
        mDiffuseAlbedoSrv, mFresenlShineseSrv,
        mSsaoSrv, mShadowDepthSrv
    };
    STContext()->PSSetShaderResources(0, 11, srvs);

    static ID3D11SamplerState* samps[] =
    {
        mPointClampSampler,
        mLinearWrapSampler,
        mShadowTexSampler
    };
    STContext()->PSSetSamplers(0, 3, samps);

    STContext()->IASetPrimitiveTopology(
        D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    STContext()->IASetVertexBuffers(
        0, 1, &mVertexBuffer,
        &stride, &offset);
    STContext()->IASetIndexBuffer(
        mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    STContext()->DrawIndexedInstanced(6, 1, 0, 0, 0);

    ID3D11RenderTargetView* rtvnull = nullptr;
    STContext()->OMSetRenderTargets(1, &rtvnull, nullptr);
    static ID3D11ShaderResourceView* nullsrvs[] =
    {
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr
    };
    STContext()->PSSetShaderResources(0, 11, nullsrvs);
}

bool RSPass_Defered::CreateShaders()
{
    ID3DBlob* shaderBlob = nullptr;
    HRESULT hr = S_OK;

    hr = Tool::CompileShaderFromFile(
        L".\\Shaders\\defered_vertex.hlsl",
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
        L".\\Shaders\\defered_pixel.hlsl",
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

bool RSPass_Defered::CreateBuffers()
{
    HRESULT hr = S_OK;
    D3D11_BUFFER_DESC bufDesc = {};

    VERTEX_INFO v[4] = {};
    v[0].mPosition = DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f);
    v[1].mPosition = DirectX::XMFLOAT3(-1.0f, +1.0f, 0.0f);
    v[2].mPosition = DirectX::XMFLOAT3(+1.0f, +1.0f, 0.0f);
    v[3].mPosition = DirectX::XMFLOAT3(+1.0f, -1.0f, 0.0f);
    v[0].mNormal = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    v[1].mNormal = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
    v[2].mNormal = DirectX::XMFLOAT3(2.0f, 0.0f, 0.0f);
    v[3].mNormal = DirectX::XMFLOAT3(3.0f, 0.0f, 0.0f);
    v[0].mTangent = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    v[1].mTangent = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
    v[2].mTangent = DirectX::XMFLOAT3(2.0f, 0.0f, 0.0f);
    v[3].mTangent = DirectX::XMFLOAT3(3.0f, 0.0f, 0.0f);
    v[0].mTexCoord = DirectX::XMFLOAT2(0.0f, 1.0f);
    v[1].mTexCoord = DirectX::XMFLOAT2(0.0f, 0.0f);
    v[2].mTexCoord = DirectX::XMFLOAT2(1.0f, 0.0f);
    v[3].mTexCoord = DirectX::XMFLOAT2(1.0f, 1.0f);
    ZeroMemory(&bufDesc, sizeof(bufDesc));
    bufDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufDesc.ByteWidth = sizeof(VERTEX_INFO) * 4;
    bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufDesc.CPUAccessFlags = 0;
    bufDesc.MiscFlags = 0;
    bufDesc.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA vinitData = {};
    ZeroMemory(&vinitData, sizeof(vinitData));
    vinitData.pSysMem = v;
    hr = Device()->CreateBuffer(
        &bufDesc, &vinitData, &mVertexBuffer);
    if (FAILED(hr)) { return false; }

    UINT indices[6] =
    {
        0, 1, 2,
        0, 2, 3
    };
    ZeroMemory(&bufDesc, sizeof(bufDesc));
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

    ZeroMemory(&bufDesc, sizeof(bufDesc));
    bufDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    bufDesc.ByteWidth = MAX_LIGHT_SIZE * sizeof(RS_LIGHT_INFO);
    bufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bufDesc.StructureByteStride = sizeof(RS_LIGHT_INFO);
    hr = Device()->CreateBuffer(
        &bufDesc, nullptr, &mLightStructedBuffer);
    if (FAILED(hr)) { return false; }

    bufDesc.ByteWidth = sizeof(Ambient);
    bufDesc.StructureByteStride = sizeof(Ambient);
    hr = Device()->CreateBuffer(
        &bufDesc, nullptr, &mAmbientStructedBuffer);
    if (FAILED(hr)) { return false; }

    bufDesc.ByteWidth = sizeof(LightInfo);
    bufDesc.StructureByteStride = sizeof(LightInfo);
    hr = Device()->CreateBuffer(
        &bufDesc, nullptr, &mLightInfoStructedBuffer);
    if (FAILED(hr)) { return false; }

    bufDesc.ByteWidth = MAX_SHADOW_SIZE * sizeof(ShadowInfo);
    bufDesc.StructureByteStride = sizeof(ShadowInfo);
    hr = Device()->CreateBuffer(
        &bufDesc, nullptr, &mShadowStructedBuffer);
    if (FAILED(hr)) { return false; }

    return true;
}

bool RSPass_Defered::CreateViews()
{
    mRenderTargetView = g_Root->Devices()->GetSwapChainRtv();

    std::string name = "mrt-worldpos";
    mWorldPosSrv = g_Root->TexturesManager()->
        GetDataTexInfo(name)->mSrv;
    name = "mrt-normal";
    mNormalSrv = g_Root->TexturesManager()->
        GetDataTexInfo(name)->mSrv;
    name = "mrt-diffuse";
    mDiffuseSrv = g_Root->TexturesManager()->
        GetDataTexInfo(name)->mSrv;
    name = "mrt-diffuse-albedo";
    mDiffuseAlbedoSrv = g_Root->TexturesManager()->
        GetDataTexInfo(name)->mSrv;
    name = "mrt-fresnel-shinese";
    mFresenlShineseSrv = g_Root->TexturesManager()->
        GetDataTexInfo(name)->mSrv;
    name = "ssao-tex-ssao";
    mSsaoSrv = g_Root->TexturesManager()->
        GetDataTexInfo(name)->mSrv;
    name = "light-depth-light-other";
    mShadowDepthSrv = g_Root->TexturesManager()->
        GetDataTexInfo(name)->mSrv;

    HRESULT hr = S_OK;
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.ElementWidth = MAX_LIGHT_SIZE;
    hr = Device()->CreateShaderResourceView(
        mLightStructedBuffer,
        &srvDesc, &mLightStructedBufferSrv);
    if (FAILED(hr)) { return false; }

    srvDesc.Buffer.ElementWidth = 1;
    hr = Device()->CreateShaderResourceView(
        mLightInfoStructedBuffer,
        &srvDesc, &mLightInfoStructedBufferSrv);
    if (FAILED(hr)) { return false; }

    hr = Device()->CreateShaderResourceView(
        mAmbientStructedBuffer,
        &srvDesc, &mAmbientStructedBufferSrv);
    if (FAILED(hr)) { return false; }

    srvDesc.Buffer.ElementWidth = MAX_SHADOW_SIZE;
    hr = Device()->CreateShaderResourceView(
        mShadowStructedBuffer,
        &srvDesc, &mShadowStructedBufferSrv);
    if (FAILED(hr)) { return false; }

    return true;
}

bool RSPass_Defered::CreateSamplers()
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
        &sampDesc, &mLinearWrapSampler);
    if (FAILED(hr)) { return false; }

    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = Device()->CreateSamplerState(
        &sampDesc, &mPointClampSampler);
    if (FAILED(hr)) { return false; }

    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = Device()->CreateSamplerState(
        &sampDesc, &mShadowTexSampler);
    if (FAILED(hr)) { return false; }

    return true;
}
