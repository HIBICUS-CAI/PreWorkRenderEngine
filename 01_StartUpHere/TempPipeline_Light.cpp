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
#include "RSCamerasContainer.h"
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

    name = "basic-shadowmap";
    RSPass_Shadow* shadow = new RSPass_Shadow(
        name, PASS_TYPE::RENDER, g_Root);
    shadow->SetExecuateOrder(1);

    name = "basic-normal";
    RSPass_Normal* normal = new RSPass_Normal(
        name, PASS_TYPE::RENDER, g_Root);
    normal->SetExecuateOrder(1);

    name = "ssao-topic";
    RSTopic* ssao_topic = new RSTopic(name);
    ssao_topic->StartTopicAssembly();
    ssao_topic->InsertPass(normal);
    ssao_topic->SetExecuateOrder(1);
    ssao_topic->FinishTopicAssembly();

    name = "shadowmap-topic";
    RSTopic* shadow_topic = new RSTopic(name);
    shadow_topic->StartTopicAssembly();
    shadow_topic->InsertPass(shadow);
    shadow_topic->SetExecuateOrder(2);
    shadow_topic->FinishTopicAssembly();

    name = "light-topic";
    RSTopic* light_topic = new RSTopic(name);
    light_topic->StartTopicAssembly();
    light_topic->InsertPass(light);
    light_topic->SetExecuateOrder(3);
    light_topic->FinishTopicAssembly();

    name = "light-pipeline";
    g_TempPipeline = new RSPipeline(name);
    g_TempPipeline->StartPipelineAssembly();
    g_TempPipeline->InsertTopic(shadow_topic);
    g_TempPipeline->InsertTopic(light_topic);
    g_TempPipeline->InsertTopic(ssao_topic);
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
    mShadowStructedBufferSrv(nullptr)
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
    mShadowStructedBufferSrv(_source.mShadowStructedBufferSrv)
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
    RS_RELEASE(mShadowTexSampler);
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
    STContext()->PSSetSamplers(1, 1, &mShadowTexSampler);
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

        STContext()->Map(mAmbientStructedBuffer, 0,
            D3D11_MAP_WRITE_DISCARD, 0, &msr);
        Ambient* amb_data = (Ambient*)msr.pData;
        amb_data[0].mAmbient = { 0.3f,0.3f,0.3f,1.f };
        STContext()->Unmap(mAmbientStructedBuffer, 0);

        STContext()->Map(mLightInfoStructedBuffer, 0,
            D3D11_MAP_WRITE_DISCARD, 0, &msr);
        LightInfo* li_data = (LightInfo*)msr.pData;
        static std::string name = "temp-cam";
        li_data[0].mCameraPos = g_Root->CamerasContainer()->
            GetRSCamera(name)->GetRSCameraPosition();
        li_data[0].mDirectLightNum = 1;
        STContext()->Unmap(mLightInfoStructedBuffer, 0);

        STContext()->Map(mLightStructedBuffer, 0,
            D3D11_MAP_WRITE_DISCARD, 0, &msr);
        RS_LIGHT_INFO* l_data = (RS_LIGHT_INFO*)msr.pData;
        // TEMP-----------------------
        l_data[0].mPosition = { 0.f,30.f,30.f };
        l_data[0].mDirection = { 0.f,-1.f,-1.f };
        l_data[0].mStrength = { 1.f,1.f,1.f };
        l_data[0].mSpotPower = 2.f;
        l_data[0].mFalloffStart = 5.f;
        l_data[0].mFalloffEnd = 15.f;
        // TEMP-----------------------
        STContext()->Unmap(mLightStructedBuffer, 0);

        STContext()->Map(mMaterialStructedBuffer, 0,
            D3D11_MAP_WRITE_DISCARD, 0, &msr);
        RS_MATERIAL_INFO* m_data = (RS_MATERIAL_INFO*)msr.pData;
        m_data[0] = call.mMaterialData;
        STContext()->Unmap(mMaterialStructedBuffer, 0);

        STContext()->Map(mShadowStructedBuffer, 0,
            D3D11_MAP_WRITE_DISCARD, 0, &msr);
        ShadowInfo* s_data = (ShadowInfo*)msr.pData;
        // TEMP---------------------
        DirectX::XMFLOAT3 pos = { 0.f,30.f,30.f };
        DirectX::XMFLOAT3 look = { 0.f,-1.f,-1.f };
        DirectX::XMFLOAT3 up = { 0.f,1.f,-1.f };
        mat = DirectX::XMMatrixLookAtLH(
            DirectX::XMLoadFloat3(&pos),
            DirectX::XMLoadFloat3(&look),
            DirectX::XMLoadFloat3(&up));
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&s_data[0].mShadowViewMat, mat);
        mat = DirectX::XMMatrixOrthographicLH(
            12.8f * 9.5f, 7.2f * 9.5f, 1.f, 100.f);
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&s_data[0].mShadowProjMat, mat);
        static DirectX::XMMATRIX T(
            0.5f, 0.0f, 0.0f, 0.0f,
            0.0f, -0.5f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.0f, 1.0f);
        mat = DirectX::XMMatrixTranspose(
            DirectX::XMLoadFloat4x4(&call.mCameraData.mViewMat) *
            DirectX::XMLoadFloat4x4(&call.mCameraData.mProjMat) *
            T);
        DirectX::XMStoreFloat4x4(&s_data[0].mSSAOMat, mat);
        // TEMP---------------------
        STContext()->Unmap(mShadowStructedBuffer, 0);

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
        STContext()->VSSetShaderResources(
            2, 1, &mShadowStructedBufferSrv);
        STContext()->PSSetShaderResources(
            0, 1, &mAmbientStructedBufferSrv);
        STContext()->PSSetShaderResources(
            1, 1, &mLightInfoStructedBufferSrv);
        STContext()->PSSetShaderResources(
            2, 1, &mMaterialStructedBufferSrv);
        STContext()->PSSetShaderResources(
            3, 1, &mLightStructedBufferSrv);
        STContext()->PSSetShaderResources(
            4, 1, &call.mTextureDatas[0].mSrv);
        static std::string depthtex = "light-depth-light";
        STContext()->PSSetShaderResources(
            5, 1, &g_Root->TexturesManager()->
            GetDataTexInfo(depthtex)->mSrv);

        STContext()->DrawIndexedInstanced(
            call.mMeshData.mIndexCount,
            (UINT)call.mInstanceData.mDataPtr->size(), 0, 0, 0);
    }

    // TEMP-----------------------------
    g_Root->Devices()->PresentSwapChain();
    mDrawCallPipe->mDatas.clear();

    ID3D11RenderTargetView* null = nullptr;
    ID3D11ShaderResourceView* nullsrv = nullptr;
    STContext()->OMSetRenderTargets(1, &null, nullptr);
    STContext()->PSSetShaderResources(0, 1, &nullsrv);
    STContext()->PSSetShaderResources(1, 1, &nullsrv);
    STContext()->PSSetShaderResources(2, 1, &nullsrv);
    STContext()->PSSetShaderResources(3, 1, &nullsrv);
    STContext()->PSSetShaderResources(4, 1, &nullsrv);
    STContext()->PSSetShaderResources(5, 1, &nullsrv);
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

    bdc.ByteWidth = sizeof(RS_MATERIAL_INFO);
    bdc.StructureByteStride = sizeof(RS_MATERIAL_INFO);
    hr = Device()->CreateBuffer(
        &bdc, nullptr, &mMaterialStructedBuffer);
    if (FAILED(hr)) { return false; }

    bdc.ByteWidth = sizeof(ShadowInfo);
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

    hr = Device()->CreateShaderResourceView(
        mMaterialStructedBuffer,
        &desSRV, &mMaterialStructedBufferSrv);
    if (FAILED(hr)) { return false; }

    hr = Device()->CreateShaderResourceView(
        mShadowStructedBuffer,
        &desSRV, &mShadowStructedBufferSrv);
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
    mRasterizerState(nullptr), mDepthStencilView(nullptr),
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

    std::string name = "light-depth-light";
    g_Root->TexturesManager()->DeleteDataTex(name);
}

void RSPass_Shadow::ExecuatePass()
{
    ID3D11RenderTargetView* null = nullptr;
    STContext()->OMSetRenderTargets(1,
        &null, mDepthStencilView);
    STContext()->ClearDepthStencilView(
        mDepthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);
    STContext()->VSSetShader(mVertexShader, nullptr, 0);
    STContext()->PSSetShader(nullptr, nullptr, 0);
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
        // TEMP---------------------
        DirectX::XMFLOAT3 pos = { 0.f,30.f,30.f };
        DirectX::XMFLOAT3 look = { 0.f,-1.f,-1.f };
        DirectX::XMFLOAT3 up = { 0.f,1.f,-1.f };
        mat = DirectX::XMMatrixLookAtLH(
            DirectX::XMLoadFloat3(&pos),
            DirectX::XMLoadFloat3(&look),
            DirectX::XMLoadFloat3(&up));
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&vp_data[0].mViewMat, mat);
        mat = DirectX::XMMatrixOrthographicLH(
            12.8f * 9.5f, 7.2f * 9.5f, 1.f, 100.f);
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&vp_data[0].mProjMat, mat);
        // TEMP---------------------
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

        STContext()->DrawIndexedInstanced(
            call.mMeshData.mIndexCount,
            (UINT)call.mInstanceData.mDataPtr->size(), 0, 0, 0);
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
    bdc.ByteWidth = 256 * sizeof(RS_INSTANCE_DATA);
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
    texDepSte.ArraySize = 1;
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
    desDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    desDSV.Texture2D.MipSlice = 0;
    hr = Device()->CreateDepthStencilView(
        depthTex, &desDSV, &mDepthStencilView);
    if (FAILED(hr)) { return false; }

    ID3D11ShaderResourceView* srv = nullptr;
    D3D11_SHADER_RESOURCE_VIEW_DESC desSRV = {};
    desSRV.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    desSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    desSRV.Texture2D.MostDetailedMip = 0;
    desSRV.Texture2D.MipLevels = 1;
    hr = Device()->CreateShaderResourceView(
        depthTex, &desSRV, &srv);
    if (FAILED(hr)) { return false; }

    DATA_TEXTURE_INFO dti = {};
    std::string name = "light-depth-light";
    dti.mTexture = depthTex;
    dti.mDsv = mDepthStencilView;
    dti.mSrv = srv;
    g_Root->TexturesManager()->AddDataTexture(name, dti);

    ZeroMemory(&desSRV, sizeof(desSRV));
    desSRV.Format = DXGI_FORMAT_UNKNOWN;
    desSRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    desSRV.Buffer.ElementWidth = 256;
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

RSPass_Normal::RSPass_Normal(
    std::string& _name, PASS_TYPE _type, RSRoot_DX11* _root) :
    RSPass_Base(_name, _type, _root),
    mVertexShader(nullptr), mRenderTargetView(nullptr),
    mRasterizerState(nullptr), mDepthStencilView(nullptr),
    mPixelShader(nullptr),
    mDrawCallType(DRAWCALL_TYPE::OPACITY),
    mDrawCallPipe(nullptr),
    mViewProjStructedBuffer(nullptr),
    mViewProjStructedBufferSrv(nullptr),
    mInstanceStructedBuffer(nullptr),
    mInstanceStructedBufferSrv(nullptr)
{

}

RSPass_Normal::RSPass_Normal(const RSPass_Normal& _source) :
    RSPass_Base(_source),
    mVertexShader(_source.mVertexShader),
    mPixelShader(_source.mPixelShader),
    mRasterizerState(_source.mRasterizerState),
    mDepthStencilView(_source.mDepthStencilView),
    mRenderTargetView(_source.mRenderTargetView),
    mDrawCallType(_source.mDrawCallType),
    mDrawCallPipe(_source.mDrawCallPipe),
    mViewProjStructedBuffer(_source.mViewProjStructedBuffer),
    mViewProjStructedBufferSrv(_source.mViewProjStructedBufferSrv),
    mInstanceStructedBuffer(_source.mInstanceStructedBuffer),
    mInstanceStructedBufferSrv(_source.mInstanceStructedBufferSrv)
{

}

RSPass_Normal::~RSPass_Normal()
{

}

RSPass_Normal* RSPass_Normal::ClonePass()
{
    return new RSPass_Normal(*this);
}

bool RSPass_Normal::InitPass()
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

void RSPass_Normal::ReleasePass()
{
    RS_RELEASE(mVertexShader);
    RS_RELEASE(mRasterizerState);
    RS_RELEASE(mViewProjStructedBufferSrv);
    RS_RELEASE(mViewProjStructedBuffer);
    RS_RELEASE(mInstanceStructedBufferSrv);
    RS_RELEASE(mInstanceStructedBuffer);

    std::string name = "normal-depth-ssao";
    g_Root->TexturesManager()->DeleteDataTex(name);
    name = "normal-tex-ssao";
    g_Root->TexturesManager()->DeleteDataTex(name);
}

void RSPass_Normal::ExecuatePass()
{
    ID3D11RenderTargetView* null = nullptr;
    STContext()->OMSetRenderTargets(1,
        &mRenderTargetView, mDepthStencilView);
    STContext()->ClearRenderTargetView(
        mRenderTargetView, DirectX::Colors::Black);
    STContext()->ClearDepthStencilView(
        mDepthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);
    STContext()->VSSetShader(mVertexShader, nullptr, 0);
    STContext()->PSSetShader(mPixelShader, nullptr, 0);
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
        STContext()->PSSetShaderResources(
            0, 1, &mViewProjStructedBufferSrv);
        STContext()->VSSetShaderResources(
            1, 1, &mInstanceStructedBufferSrv);

        STContext()->DrawIndexedInstanced(
            call.mMeshData.mIndexCount,
            (UINT)call.mInstanceData.mDataPtr->size(), 0, 0, 0);
    }

    STContext()->OMSetRenderTargets(1, &null, nullptr);
    STContext()->RSSetState(nullptr);
}

bool RSPass_Normal::CreateShaders()
{
    ID3DBlob* shaderBlob = nullptr;
    HRESULT hr = S_OK;

    hr = Tool::CompileShaderFromFile(
        L".\\Shaders\\normal_vertex.hlsl",
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
        L".\\Shaders\\normal_pixel.hlsl",
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

bool RSPass_Normal::CreateStates()
{
    return true;
}

bool RSPass_Normal::CreateBuffers()
{
    HRESULT hr = S_OK;
    D3D11_BUFFER_DESC bdc = {};

    ZeroMemory(&bdc, sizeof(bdc));
    bdc.Usage = D3D11_USAGE_DYNAMIC;
    bdc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bdc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    bdc.ByteWidth = 256 * sizeof(RS_INSTANCE_DATA);
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

bool RSPass_Normal::CreateViews()
{
    HRESULT hr = S_OK;
    ID3D11Texture2D* depthTex = nullptr;
    D3D11_TEXTURE2D_DESC texDepSte = {};
    texDepSte.Width = 1280;
    texDepSte.Height = 720;
    texDepSte.MipLevels = 1;
    texDepSte.ArraySize = 1;
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
    desDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    desDSV.Texture2D.MipSlice = 0;
    hr = Device()->CreateDepthStencilView(
        depthTex, &desDSV, &mDepthStencilView);
    if (FAILED(hr)) { return false; }

    ID3D11ShaderResourceView* srv = nullptr;
    D3D11_SHADER_RESOURCE_VIEW_DESC desSRV = {};
    desSRV.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    desSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    desSRV.Texture2D.MostDetailedMip = 0;
    desSRV.Texture2D.MipLevels = 1;
    hr = Device()->CreateShaderResourceView(
        depthTex, &desSRV, &srv);
    if (FAILED(hr)) { return false; }

    DATA_TEXTURE_INFO dti = {};
    std::string name = "normal-depth-ssao";
    dti.mTexture = depthTex;
    dti.mDsv = mDepthStencilView;
    dti.mSrv = srv;
    g_Root->TexturesManager()->AddDataTexture(name, dti);

    ID3D11Texture2D* normalTex = nullptr;
    D3D11_TEXTURE2D_DESC texDesc = {};
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
        &texDesc, nullptr, &normalTex);
    if (FAILED(hr)) { return false; }

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    rtvDesc.Texture2D.MipSlice = 0;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    hr = Device()->CreateRenderTargetView(
        normalTex, &rtvDesc, &mRenderTargetView);
    if (FAILED(hr)) { return false; }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    hr = Device()->CreateShaderResourceView(normalTex,
        &srvDesc, &srv);
    if (FAILED(hr)) { return false; }

    dti = {};
    name = "normal-tex-ssao";
    dti.mTexture = normalTex;
    dti.mRtv = mRenderTargetView;
    dti.mSrv = srv;
    g_Root->TexturesManager()->AddDataTexture(name, dti);

    ZeroMemory(&desSRV, sizeof(desSRV));
    desSRV.Format = DXGI_FORMAT_UNKNOWN;
    desSRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    desSRV.Buffer.ElementWidth = 256;
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

bool RSPass_Normal::CreateSamplers()
{
    return true;
}
