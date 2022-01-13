#include "TempTopic_Particle.h"
#include "RSRoot_DX11.h"
#include "RSDevices.h"
#include "RSResourceManager.h"
#include "RSUtilityFunctions.h"
#include "RSShaderCompile.h"
#include "RSParticlesContainer.h"
#include "RSCamerasContainer.h"
#include "DDSTextureLoader11.h"

#define RS_RELEASE(p) { if (p) { (p)->Release(); (p)=nullptr; } }

static RSPass_PriticleSetUp* g_ParticleSetUpPass = nullptr;

RSPass_PriticleSetUp::RSPass_PriticleSetUp(
    std::string& _name, PASS_TYPE _type, RSRoot_DX11* _root) :
    RSPass_Base(_name, _type, _root),
    mTilingConstant({}),
    mParticleRenderBuffer(nullptr),
    mParticleRender_Srv(nullptr), mParticleRender_Uav(nullptr),
    mParticleRandomTexture(nullptr), mParticleRandom_Srv(nullptr),
    mParticlePartA(nullptr), mPartA_Srv(nullptr), mPartA_Uav(nullptr),
    mParticlePartB(nullptr), mPartB_Uav(nullptr),
    mViewspacePosBuffer(nullptr),
    mViewSpacePos_Srv(nullptr), mViewSpacePos_Uav(nullptr),
    mMaxRadiusBuffer(nullptr),
    mMaxRadius_Srv(nullptr), mMaxRadius_Uav(nullptr),
    mStridedCoarseCullBuffer(nullptr),
    mStridedCoarseCull_Srv(nullptr), mStridedCoarseCull_Uav(nullptr),
    mStridedCoarseCullCounterBuffer(nullptr),
    mStridedCoarseCullCounter_Srv(nullptr),
    mStridedCoarseCullCounter_Uav(nullptr),
    mTiledIndexBuffer(nullptr),
    mTiledIndex_Srv(nullptr),
    mTiledIndex_Uav(nullptr),
    mDeadListBuffer(nullptr), mDeadList_Uav(nullptr),
    mAliveIndexBuffer(nullptr),
    mAliveIndex_Srv(nullptr), mAliveIndex_Uav(nullptr),
    mDeadListConstantBuffer(nullptr),
    mActiveListConstantBuffer(nullptr),
    mEmitterConstantBuffer(nullptr),
    mCameraConstantBuffer(nullptr),
    mTilingConstantBuffer(nullptr),
    mDebugCounterBuffer(nullptr),
    mSimulEmitterStructedBuffer(nullptr),
    mSimulEmitterStructedBuffer_Srv(nullptr),
    mTimeConstantBuffer(nullptr)
{
    g_ParticleSetUpPass = this;
}

RSPass_PriticleSetUp::RSPass_PriticleSetUp(
    const RSPass_PriticleSetUp& _source) :
    RSPass_Base(_source),
    mTilingConstant(_source.mTilingConstant),
    mParticleRenderBuffer(_source.mParticleRenderBuffer),
    mParticleRender_Srv(_source.mParticleRender_Srv),
    mParticleRender_Uav(_source.mParticleRender_Uav),
    mParticleRandomTexture(_source.mParticleRandomTexture),
    mParticleRandom_Srv(_source.mParticleRandom_Srv),
    mParticlePartA(_source.mParticlePartA),
    mPartA_Srv(_source.mPartA_Srv),
    mPartA_Uav(_source.mPartA_Uav),
    mParticlePartB(_source.mParticlePartB),
    mPartB_Uav(_source.mPartB_Uav),
    mViewspacePosBuffer(_source.mViewspacePosBuffer),
    mViewSpacePos_Srv(_source.mViewSpacePos_Srv),
    mViewSpacePos_Uav(_source.mViewSpacePos_Uav),
    mMaxRadiusBuffer(_source.mMaxRadiusBuffer),
    mMaxRadius_Srv(_source.mMaxRadius_Srv),
    mMaxRadius_Uav(_source.mMaxRadius_Uav),
    mStridedCoarseCullBuffer(_source.mStridedCoarseCullBuffer),
    mStridedCoarseCull_Srv(_source.mStridedCoarseCull_Srv),
    mStridedCoarseCull_Uav(_source.mStridedCoarseCull_Uav),
    mStridedCoarseCullCounterBuffer(_source.mStridedCoarseCullCounterBuffer),
    mStridedCoarseCullCounter_Srv(_source.mStridedCoarseCullCounter_Srv),
    mStridedCoarseCullCounter_Uav(_source.mStridedCoarseCullCounter_Uav),
    mTiledIndexBuffer(_source.mTiledIndexBuffer),
    mTiledIndex_Srv(_source.mTiledIndex_Srv),
    mTiledIndex_Uav(_source.mTiledIndex_Uav),
    mDeadListBuffer(_source.mDeadListBuffer),
    mDeadList_Uav(_source.mDeadList_Uav),
    mAliveIndexBuffer(_source.mAliveIndexBuffer),
    mAliveIndex_Srv(_source.mAliveIndex_Srv),
    mAliveIndex_Uav(_source.mAliveIndex_Uav),
    mDeadListConstantBuffer(_source.mDeadListConstantBuffer),
    mActiveListConstantBuffer(_source.mActiveListConstantBuffer),
    mEmitterConstantBuffer(_source.mEmitterConstantBuffer),
    mCameraConstantBuffer(_source.mCameraConstantBuffer),
    mTilingConstantBuffer(_source.mTilingConstantBuffer),
    mDebugCounterBuffer(_source.mDebugCounterBuffer),
    mSimulEmitterStructedBuffer(_source.mSimulEmitterStructedBuffer),
    mSimulEmitterStructedBuffer_Srv(_source.mSimulEmitterStructedBuffer_Srv),
    mTimeConstantBuffer(_source.mTimeConstantBuffer)
{
    g_ParticleSetUpPass = this;
}

RSPass_PriticleSetUp::~RSPass_PriticleSetUp()
{

}

const RS_TILING_CONSTANT& RSPass_PriticleSetUp::GetTilingConstantInfo() const
{
    return mTilingConstant;
}

RSPass_PriticleSetUp* RSPass_PriticleSetUp::ClonePass()
{
    return new RSPass_PriticleSetUp(*this);
}

bool RSPass_PriticleSetUp::InitPass()
{
    int width = GetRSRoot_DX11_Singleton()->Devices()->GetCurrWndWidth();
    int height = GetRSRoot_DX11_Singleton()->Devices()->GetCurrWndHeight();

    mTilingConstant.mNumTilesX =
        Tool::Align(width, PTC_TILE_X_SIZE) / PTC_TILE_X_SIZE;
    mTilingConstant.mNumTilesY =
        Tool::Align(height, PTC_TILE_Y_SIZE) / PTC_TILE_Y_SIZE;
    mTilingConstant.mNumCoarseCullingTilesX = PTC_MAX_COARSE_CULL_TILE_X;
    mTilingConstant.mNumCoarseCullingTilesY = PTC_MAX_COARSE_CULL_TILE_Y;
    mTilingConstant.mNumCullingTilesPerCoarseTileX =
        Tool::Align(
            mTilingConstant.mNumTilesX,
            mTilingConstant.mNumCoarseCullingTilesX) /
        mTilingConstant.mNumCoarseCullingTilesX;
    mTilingConstant.mNumCullingTilesPerCoarseTileY =
        Tool::Align(
            mTilingConstant.mNumTilesY,
            mTilingConstant.mNumCoarseCullingTilesY) /
        mTilingConstant.mNumCoarseCullingTilesY;

    if (!CreateBuffers()) { return false; }
    if (!CreateViews()) { return false; }

    auto resManager = GetRSRoot_DX11_Singleton()->ResourceManager();
    RS_RESOURCE_INFO res;
    std::string name = "";

    res = {};
    res.mType = RS_RESOURCE_TYPE::BUFFER;
    res.mResource.mBuffer = mParticleRenderBuffer;
    res.mSrv = mParticleRender_Srv;
    res.mUav = mParticleRender_Uav;
    name = PTC_RENDER_BUFFER_NAME;
    resManager->AddResource(name, res);

    res = {};
    res.mType = RS_RESOURCE_TYPE::BUFFER;
    res.mResource.mBuffer = mParticlePartA;
    res.mSrv = mPartA_Srv;
    res.mUav = mPartA_Uav;
    name = PTC_A_NAME;
    resManager->AddResource(name, res);

    res = {};
    res.mType = RS_RESOURCE_TYPE::BUFFER;
    res.mResource.mBuffer = mParticlePartB;
    res.mUav = mPartB_Uav;
    name = PTC_B_NAME;
    resManager->AddResource(name, res);

    res = {};
    res.mType = RS_RESOURCE_TYPE::BUFFER;
    res.mResource.mBuffer = mViewspacePosBuffer;
    res.mSrv = mViewSpacePos_Srv;
    res.mUav = mViewSpacePos_Uav;
    name = PTC_VIEW_SPCACE_POS_NAME;
    resManager->AddResource(name, res);

    res = {};
    res.mType = RS_RESOURCE_TYPE::BUFFER;
    res.mResource.mBuffer = mMaxRadiusBuffer;
    res.mSrv = mMaxRadius_Srv;
    res.mUav = mMaxRadius_Uav;
    name = PTC_MAX_RADIUS_NAME;
    resManager->AddResource(name, res);

    res = {};
    res.mType = RS_RESOURCE_TYPE::BUFFER;
    res.mResource.mBuffer = mStridedCoarseCullBuffer;
    res.mSrv = mStridedCoarseCull_Srv;
    res.mUav = mStridedCoarseCull_Uav;
    name = PTC_COARSE_CULL_NAME;
    resManager->AddResource(name, res);

    res = {};
    res.mType = RS_RESOURCE_TYPE::BUFFER;
    res.mResource.mBuffer = mStridedCoarseCullCounterBuffer;
    res.mSrv = mStridedCoarseCullCounter_Srv;
    res.mUav = mStridedCoarseCullCounter_Uav;
    name = PTC_COARSE_CULL_COUNTER_NAME;
    resManager->AddResource(name, res);

    res = {};
    res.mType = RS_RESOURCE_TYPE::BUFFER;
    res.mResource.mBuffer = mTiledIndexBuffer;
    res.mSrv = mTiledIndex_Srv;
    res.mUav = mTiledIndex_Uav;
    name = PTC_TILED_INDEX_NAME;
    resManager->AddResource(name, res);

    res = {};
    res.mType = RS_RESOURCE_TYPE::BUFFER;
    res.mResource.mBuffer = mDeadListBuffer;
    res.mUav = mDeadList_Uav;
    name = PTC_DEAD_LIST_NAME;
    resManager->AddResource(name, res);

    res = {};
    res.mType = RS_RESOURCE_TYPE::BUFFER;
    res.mResource.mBuffer = mAliveIndexBuffer;
    res.mSrv = mAliveIndex_Srv;
    res.mUav = mAliveIndex_Uav;
    name = PTC_ALIVE_INDEX_NAME;
    resManager->AddResource(name, res);

    res = {};
    res.mType = RS_RESOURCE_TYPE::BUFFER;
    res.mResource.mBuffer = mDeadListConstantBuffer;
    name = PTC_DEAD_LIST_CONSTANT_NAME;
    resManager->AddResource(name, res);

    res = {};
    res.mType = RS_RESOURCE_TYPE::BUFFER;
    res.mResource.mBuffer = mActiveListConstantBuffer;
    name = PTC_ALIVE_LIST_CONSTANT_NAME;
    resManager->AddResource(name, res);

    res = {};
    res.mType = RS_RESOURCE_TYPE::BUFFER;
    res.mResource.mBuffer = mEmitterConstantBuffer;
    name = PTC_EMITTER_CONSTANT_NAME;
    resManager->AddResource(name, res);

    res = {};
    res.mType = RS_RESOURCE_TYPE::BUFFER;
    res.mResource.mBuffer = mCameraConstantBuffer;
    name = PTC_CAMERA_CONSTANT_NAME;
    resManager->AddResource(name, res);

    res = {};
    res.mType = RS_RESOURCE_TYPE::BUFFER;
    res.mResource.mBuffer = mTilingConstantBuffer;
    name = PTC_TILING_CONSTANT_NAME;
    resManager->AddResource(name, res);

    res = {};
    res.mType = RS_RESOURCE_TYPE::BUFFER;
    res.mResource.mBuffer = mDebugCounterBuffer;
    name = PTC_DEBUG_COUNTER_NAME;
    resManager->AddResource(name, res);

    res = {};
    res.mType = RS_RESOURCE_TYPE::TEXTURE2D;
    res.mResource.mTexture2D = mParticleRandomTexture;
    res.mSrv = mParticleRandom_Srv;
    name = PTC_RAMDOM_TEXTURE_NAME;
    resManager->AddResource(name, res);

    res = {};
    res.mType = RS_RESOURCE_TYPE::BUFFER;
    res.mResource.mBuffer = mSimulEmitterStructedBuffer;
    res.mSrv = mSimulEmitterStructedBuffer_Srv;
    name = PTC_SIMU_EMITTER_STRU_NAME;
    resManager->AddResource(name, res);

    res = {};
    res.mType = RS_RESOURCE_TYPE::BUFFER;
    res.mResource.mBuffer = mTimeConstantBuffer;
    name = PTC_TIME_CONSTANT_NAME;
    resManager->AddResource(name, res);

    return true;
}

void RSPass_PriticleSetUp::ReleasePass()
{
    auto resManager = GetRSRoot_DX11_Singleton()->ResourceManager();
    std::string name = PTC_RENDER_BUFFER_NAME;
    resManager->DeleteResource(name);
    name = PTC_A_NAME;
    resManager->DeleteResource(name);
    name = PTC_B_NAME;
    resManager->DeleteResource(name);
    name = PTC_VIEW_SPCACE_POS_NAME;
    resManager->DeleteResource(name);
    name = PTC_MAX_RADIUS_NAME;
    resManager->DeleteResource(name);
    name = PTC_COARSE_CULL_NAME;
    resManager->DeleteResource(name);
    name = PTC_COARSE_CULL_COUNTER_NAME;
    resManager->DeleteResource(name);
    name = PTC_TILED_INDEX_NAME;
    resManager->DeleteResource(name);
    name = PTC_DEAD_LIST_NAME;
    resManager->DeleteResource(name);
    name = PTC_ALIVE_INDEX_NAME;
    resManager->DeleteResource(name);
    name = PTC_DEAD_LIST_CONSTANT_NAME;
    resManager->DeleteResource(name);
    name = PTC_ALIVE_LIST_CONSTANT_NAME;
    resManager->DeleteResource(name);
    name = PTC_EMITTER_CONSTANT_NAME;
    resManager->DeleteResource(name);
    name = PTC_CAMERA_CONSTANT_NAME;
    resManager->DeleteResource(name);
    name = PTC_TILING_CONSTANT_NAME;
    resManager->DeleteResource(name);
    name = PTC_DEBUG_COUNTER_NAME;
    resManager->DeleteResource(name);
    name = PTC_RAMDOM_TEXTURE_NAME;
    resManager->DeleteResource(name);
    name = PTC_SIMU_EMITTER_STRU_NAME;
    resManager->DeleteResource(name);
    name = PTC_TIME_CONSTANT_NAME;
    resManager->DeleteResource(name);
}

void RSPass_PriticleSetUp::ExecuatePass()
{

}

bool RSPass_PriticleSetUp::CreateBuffers()
{
    HRESULT hr = S_OK;
    D3D11_BUFFER_DESC bfrDesc = {};
    D3D11_TEXTURE2D_DESC texDesc = {};
    ZeroMemory(&bfrDesc, sizeof(bfrDesc));
    ZeroMemory(&texDesc, sizeof(texDesc));

    bfrDesc.ByteWidth = sizeof(RS_PARTICLE_PART_A) * PTC_MAX_PARTICLE_SIZE;
    bfrDesc.Usage = D3D11_USAGE_DEFAULT;
    bfrDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    bfrDesc.CPUAccessFlags = 0;
    bfrDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bfrDesc.StructureByteStride = sizeof(RS_PARTICLE_PART_A);
    hr = Device()->CreateBuffer(&bfrDesc, nullptr, &mParticlePartA);
    if (FAILED(hr)) { return false; }

    bfrDesc.ByteWidth = sizeof(RS_PARTICLE_PART_B) * PTC_MAX_PARTICLE_SIZE;
    bfrDesc.StructureByteStride = sizeof(RS_PARTICLE_PART_B);
    hr = Device()->CreateBuffer(&bfrDesc, nullptr, &mParticlePartB);
    if (FAILED(hr)) { return false; }

    bfrDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * PTC_MAX_PARTICLE_SIZE;
    bfrDesc.StructureByteStride = sizeof(DirectX::XMFLOAT4);
    hr = Device()->CreateBuffer(&bfrDesc, nullptr, &mViewspacePosBuffer);
    if (FAILED(hr)) { return false; }

    bfrDesc.ByteWidth = sizeof(float) * PTC_MAX_PARTICLE_SIZE;
    bfrDesc.StructureByteStride = sizeof(float);
    hr = Device()->CreateBuffer(&bfrDesc, nullptr, &mMaxRadiusBuffer);
    if (FAILED(hr)) { return false; }

    bfrDesc.ByteWidth = sizeof(UINT) * PTC_MAX_PARTICLE_SIZE;
    bfrDesc.StructureByteStride = sizeof(UINT);
    hr = Device()->CreateBuffer(&bfrDesc, nullptr, &mDeadListBuffer);
    if (FAILED(hr)) { return false; }

    bfrDesc.ByteWidth =
        sizeof(RS_ALIVE_INDEX_BUFFER_ELEMENT) * PTC_MAX_PARTICLE_SIZE;
    bfrDesc.StructureByteStride = sizeof(RS_ALIVE_INDEX_BUFFER_ELEMENT);
    hr = Device()->CreateBuffer(&bfrDesc, nullptr, &mAliveIndexBuffer);
    if (FAILED(hr)) { return false; }

    bfrDesc.StructureByteStride = 0;
    bfrDesc.MiscFlags = 0;
    bfrDesc.ByteWidth =
        sizeof(UINT) * PTC_MAX_PARTICLE_SIZE * PTC_MAX_COARSE_CULL_TILE_SIZE;
    hr = Device()->CreateBuffer(&bfrDesc, nullptr, &mStridedCoarseCullBuffer);
    if (FAILED(hr)) { return false; }

    bfrDesc.ByteWidth = sizeof(UINT) * PTC_MAX_COARSE_CULL_TILE_SIZE;
    hr = Device()->CreateBuffer(&bfrDesc, nullptr,
        &mStridedCoarseCullCounterBuffer);
    if (FAILED(hr)) { return false; }

    UINT numElements = mTilingConstant.mNumTilesX * mTilingConstant.mNumTilesY *
        PTC_TILE_BUFFER_SIZE;
    bfrDesc.ByteWidth = sizeof(UINT) * numElements;
    hr = Device()->CreateBuffer(&bfrDesc, nullptr, &mTiledIndexBuffer);
    if (FAILED(hr)) { return false; }

    numElements =
        mTilingConstant.mNumTilesX * mTilingConstant.mNumTilesY *
        PTC_TILE_X_SIZE * PTC_TILE_Y_SIZE;
    bfrDesc.ByteWidth = 8 * numElements;    // DXGI_FORMAT_R16G16B16A16_FLOAT
    hr = Device()->CreateBuffer(&bfrDesc, nullptr, &mParticleRenderBuffer);
    if (FAILED(hr)) { return false; }

    ZeroMemory(&bfrDesc, sizeof(bfrDesc));
    bfrDesc.Usage = D3D11_USAGE_DEFAULT;
    bfrDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bfrDesc.CPUAccessFlags = 0;
    bfrDesc.ByteWidth = 4 * sizeof(UINT);   // one for record and three for pad
    hr = Device()->CreateBuffer(&bfrDesc, nullptr, &mDeadListConstantBuffer);
    if (FAILED(hr)) { return false; }
    hr = Device()->CreateBuffer(&bfrDesc, nullptr, &mActiveListConstantBuffer);
    if (FAILED(hr)) { return false; }

    ZeroMemory(&bfrDesc, sizeof(bfrDesc));
    bfrDesc.Usage = D3D11_USAGE_DYNAMIC;
    bfrDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bfrDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bfrDesc.ByteWidth = sizeof(RS_PARTICLE_EMITTER_INFO);
    hr = Device()->CreateBuffer(&bfrDesc, nullptr, &mEmitterConstantBuffer);
    if (FAILED(hr)) { return false; }

    bfrDesc.ByteWidth = sizeof(CAMERA_STATUS);
    hr = Device()->CreateBuffer(&bfrDesc, nullptr, &mCameraConstantBuffer);
    if (FAILED(hr)) { return false; }

    bfrDesc.ByteWidth = sizeof(RS_TILING_CONSTANT);
    hr = Device()->CreateBuffer(&bfrDesc, nullptr, &mTilingConstantBuffer);
    if (FAILED(hr)) { return false; }

    bfrDesc.ByteWidth = sizeof(PTC_TIME_CONSTANT);
    hr = Device()->CreateBuffer(&bfrDesc, nullptr, &mTimeConstantBuffer);
    if (FAILED(hr)) { return false; }

    ZeroMemory(&bfrDesc, sizeof(bfrDesc));
    bfrDesc.Usage = D3D11_USAGE_STAGING;
    bfrDesc.BindFlags = 0;
    bfrDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    bfrDesc.ByteWidth = sizeof(UINT);
    hr = Device()->CreateBuffer(&bfrDesc, nullptr, &mDebugCounterBuffer);
    if (FAILED(hr)) { return false; }

    ZeroMemory(&bfrDesc, sizeof(bfrDesc));
    bfrDesc.Usage = D3D11_USAGE_DYNAMIC;
    bfrDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bfrDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    bfrDesc.ByteWidth = MAX_PARTICLE_EMITTER_SIZE *
        sizeof(SIMULATE_EMITTER_INFO);
    bfrDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bfrDesc.StructureByteStride = sizeof(SIMULATE_EMITTER_INFO);
    hr = Device()->CreateBuffer(
        &bfrDesc, nullptr, &mSimulEmitterStructedBuffer);
    if (FAILED(hr)) { return false; }

    texDesc.Width = 1024;
    texDesc.Height = 1024;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    texDesc.Usage = D3D11_USAGE_IMMUTABLE;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texDesc.MipLevels = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;

    float* values = new float[texDesc.Width * texDesc.Height * 4];
    float* ptr = values;
    for (UINT i = 0; i < texDesc.Width * texDesc.Height; i++)
    {
        ptr[0] = Tool::RandomVariance(0.0f, 1.0f);
        ptr[1] = Tool::RandomVariance(0.0f, 1.0f);
        ptr[2] = Tool::RandomVariance(0.0f, 1.0f);
        ptr[3] = Tool::RandomVariance(0.0f, 1.0f);
        ptr += 4;
    }

    D3D11_SUBRESOURCE_DATA data = {};
    data.pSysMem = values;
    data.SysMemPitch = texDesc.Width * 16;
    data.SysMemSlicePitch = 0;

    hr = Device()->CreateTexture2D(&texDesc, &data, &mParticleRandomTexture);
    delete[] values;
    if (FAILED(hr)) { return false; }

    return true;
}

bool RSPass_PriticleSetUp::CreateViews()
{
    HRESULT hr = S_OK;
    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    ZeroMemory(&uavDesc, sizeof(uavDesc));
    ZeroMemory(&srvDesc, sizeof(srvDesc));

    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.ElementOffset = 0;
    srvDesc.Buffer.ElementWidth = PTC_MAX_PARTICLE_SIZE;
    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.FirstElement = 0;
    uavDesc.Buffer.NumElements = PTC_MAX_PARTICLE_SIZE;
    uavDesc.Buffer.Flags = 0;
    hr = Device()->CreateShaderResourceView(
        mParticlePartA, &srvDesc, &mPartA_Srv);
    if (FAILED(hr)) { return false; }
    hr = Device()->CreateUnorderedAccessView(
        mParticlePartA, &uavDesc, &mPartA_Uav);
    if (FAILED(hr)) { return false; }
    hr = Device()->CreateUnorderedAccessView(
        mParticlePartB, &uavDesc, &mPartB_Uav);
    if (FAILED(hr)) { return false; }

    hr = Device()->CreateShaderResourceView(
        mViewspacePosBuffer, &srvDesc, &mViewSpacePos_Srv);
    if (FAILED(hr)) { return false; }
    hr = Device()->CreateUnorderedAccessView(
        mViewspacePosBuffer, &uavDesc, &mViewSpacePos_Uav);
    if (FAILED(hr)) { return false; }

    hr = Device()->CreateShaderResourceView(
        mMaxRadiusBuffer, &srvDesc, &mMaxRadius_Srv);
    if (FAILED(hr)) { return false; }
    hr = Device()->CreateUnorderedAccessView(
        mMaxRadiusBuffer, &uavDesc, &mMaxRadius_Uav);
    if (FAILED(hr)) { return false; }

    uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
    hr = Device()->CreateUnorderedAccessView(
        mDeadListBuffer, &uavDesc, &mDeadList_Uav);
    if (FAILED(hr)) { return false; }

    uavDesc.Format = DXGI_FORMAT_R32_UINT;
    uavDesc.Buffer.Flags = 0;
    uavDesc.Buffer.NumElements =
        PTC_MAX_PARTICLE_SIZE * PTC_MAX_COARSE_CULL_TILE_SIZE;
    srvDesc.Format = DXGI_FORMAT_R32_UINT;
    srvDesc.Buffer.NumElements =
        PTC_MAX_PARTICLE_SIZE * PTC_MAX_COARSE_CULL_TILE_SIZE;
    hr = Device()->CreateShaderResourceView(
        mStridedCoarseCullBuffer, &srvDesc, &mStridedCoarseCull_Srv);
    if (FAILED(hr)) { return false; }
    hr = Device()->CreateUnorderedAccessView(
        mStridedCoarseCullBuffer, &uavDesc, &mStridedCoarseCull_Uav);
    if (FAILED(hr)) { return false; }

    uavDesc.Buffer.NumElements = PTC_MAX_COARSE_CULL_TILE_SIZE;
    srvDesc.Buffer.NumElements = PTC_MAX_COARSE_CULL_TILE_SIZE;
    hr = Device()->CreateShaderResourceView(
        mStridedCoarseCullCounterBuffer, &srvDesc,
        &mStridedCoarseCullCounter_Srv);
    if (FAILED(hr)) { return false; }
    hr = Device()->CreateUnorderedAccessView(
        mStridedCoarseCullCounterBuffer, &uavDesc,
        &mStridedCoarseCullCounter_Uav);
    if (FAILED(hr)) { return false; }

    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.ElementOffset = 0;
    srvDesc.Buffer.ElementWidth = PTC_MAX_PARTICLE_SIZE;
    uavDesc.Buffer.NumElements = PTC_MAX_PARTICLE_SIZE;
    uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
    hr = Device()->CreateShaderResourceView(
        mAliveIndexBuffer, &srvDesc, &mAliveIndex_Srv);
    if (FAILED(hr)) { return false; }
    hr = Device()->CreateUnorderedAccessView(
        mAliveIndexBuffer, &uavDesc, &mAliveIndex_Uav);
    if (FAILED(hr)) { return false; }

    UINT numElements = mTilingConstant.mNumTilesX * mTilingConstant.mNumTilesY *
        PTC_TILE_BUFFER_SIZE;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    ZeroMemory(&uavDesc, sizeof(uavDesc));
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.ElementOffset = 0;
    srvDesc.Format = DXGI_FORMAT_R32_UINT;
    srvDesc.Buffer.ElementWidth = numElements;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.FirstElement = 0;
    uavDesc.Format = DXGI_FORMAT_R32_UINT;
    uavDesc.Buffer.NumElements = numElements;
    hr = Device()->CreateShaderResourceView(
        mTiledIndexBuffer, &srvDesc, &mTiledIndex_Srv);
    if (FAILED(hr)) { return false; }
    hr = Device()->CreateUnorderedAccessView(
        mTiledIndexBuffer, &uavDesc, &mTiledIndex_Uav);
    if (FAILED(hr)) { return false; }

    numElements =
        mTilingConstant.mNumTilesX * mTilingConstant.mNumTilesY *
        PTC_TILE_X_SIZE * PTC_TILE_Y_SIZE;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    ZeroMemory(&uavDesc, sizeof(uavDesc));
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.ElementOffset = 0;
    srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    srvDesc.Buffer.ElementWidth = numElements;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.FirstElement = 0;
    uavDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    uavDesc.Buffer.NumElements = numElements;
    hr = Device()->CreateShaderResourceView(
        mParticleRenderBuffer, &srvDesc, &mParticleRender_Srv);
    if (FAILED(hr)) { return false; }
    hr = Device()->CreateUnorderedAccessView(
        mParticleRenderBuffer, &uavDesc, &mParticleRender_Uav);
    if (FAILED(hr)) { return false; }

    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;
    hr = Device()->CreateShaderResourceView(
        mParticleRandomTexture, &srvDesc, &mParticleRandom_Srv);
    if (FAILED(hr)) { return false; }

    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.ElementWidth = MAX_PARTICLE_EMITTER_SIZE;
    hr = Device()->CreateShaderResourceView(
        mSimulEmitterStructedBuffer,
        &srvDesc, &mSimulEmitterStructedBuffer_Srv);
    if (FAILED(hr)) { return false; }

    return true;
}

RSPass_PriticleEmitSimulate::RSPass_PriticleEmitSimulate(
    std::string& _name, PASS_TYPE _type, RSRoot_DX11* _root) :
    RSPass_Base(_name, _type, _root),
    mRSParticleContainerPtr(nullptr),
    mInitDeadListShader(nullptr), mResetParticlesShader(nullptr),
    mEmitParticleShader(nullptr), mSimulateShader(nullptr),
    mDeadList_Uav(nullptr), mPartA_Uav(nullptr), mPartB_Uav(nullptr),
    mRandomTex_Srv(nullptr), mEmitterConstantBuffer(nullptr),
    mDeadListConstantBuffer(nullptr),
    mLinearWrapSampler(nullptr),
    mDepthTex_Srv(nullptr), mSimulEmitterStructedBuffer_Srv(nullptr),
    mAliveIndex_Uav(nullptr), mViewSpacePos_Uav(nullptr),
    mMaxRadius_Uav(nullptr), mSimulEmitterStructedBuffer(nullptr),
    mRSCameraInfo(nullptr), mCameraConstantBuffer(nullptr),
    mTimeConstantBuffer(nullptr)
{

}

RSPass_PriticleEmitSimulate::RSPass_PriticleEmitSimulate(
    const RSPass_PriticleEmitSimulate& _source) :
    RSPass_Base(_source),
    mRSParticleContainerPtr(_source.mRSParticleContainerPtr),
    mInitDeadListShader(_source.mInitDeadListShader),
    mResetParticlesShader(_source.mResetParticlesShader),
    mEmitParticleShader(_source.mEmitParticleShader),
    mSimulateShader(_source.mSimulateShader),
    mDeadList_Uav(_source.mDeadList_Uav),
    mPartA_Uav(_source.mPartA_Uav), mPartB_Uav(_source.mPartB_Uav),
    mRandomTex_Srv(_source.mRandomTex_Srv),
    mEmitterConstantBuffer(_source.mEmitterConstantBuffer),
    mDeadListConstantBuffer(_source.mDeadListConstantBuffer),
    mLinearWrapSampler(_source.mLinearWrapSampler),
    mDepthTex_Srv(_source.mDepthTex_Srv),
    mSimulEmitterStructedBuffer_Srv(_source.mSimulEmitterStructedBuffer_Srv),
    mAliveIndex_Uav(_source.mAliveIndex_Uav),
    mViewSpacePos_Uav(_source.mViewSpacePos_Uav),
    mMaxRadius_Uav(_source.mMaxRadius_Uav),
    mSimulEmitterStructedBuffer(_source.mSimulEmitterStructedBuffer),
    mRSCameraInfo(_source.mRSCameraInfo),
    mCameraConstantBuffer(_source.mCameraConstantBuffer),
    mTimeConstantBuffer(_source.mTimeConstantBuffer)
{

}

RSPass_PriticleEmitSimulate::~RSPass_PriticleEmitSimulate()
{

}

RSPass_PriticleEmitSimulate* RSPass_PriticleEmitSimulate::ClonePass()
{
    return new RSPass_PriticleEmitSimulate(*this);
}

bool RSPass_PriticleEmitSimulate::InitPass()
{
    mRSParticleContainerPtr = GetRSRoot_DX11_Singleton()->ParticlesContainer();
    if (!mRSParticleContainerPtr) { return false; }

    std::string name = "temp-cam";
    mRSCameraInfo = GetRSRoot_DX11_Singleton()->CamerasContainer()->
        GetRSCameraInfo(name);
    if (!mRSCameraInfo) { return false; }

    if (!CreateShaders()) { return false; }
    if (!CreateSampler()) { return false; }
    if (!CheckResources()) { return false; }

    mRSParticleContainerPtr->ResetRSParticleSystem();

    return true;
}

void RSPass_PriticleEmitSimulate::ReleasePass()
{
    RS_RELEASE(mSimulateShader);
    RS_RELEASE(mEmitParticleShader);
    RS_RELEASE(mResetParticlesShader);
    RS_RELEASE(mInitDeadListShader);
}

void RSPass_PriticleEmitSimulate::ExecuatePass()
{
    if (mRSParticleContainerPtr->GetResetFlg())
    {
        {
            STContext()->CSSetShader(mInitDeadListShader,
                nullptr, 0);
            ID3D11UnorderedAccessView* uav[] = { mDeadList_Uav };
            UINT initialCount[] = { 0 };
            STContext()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uav),
                uav, initialCount);

            STContext()->Dispatch(
                Tool::Align(PTC_MAX_PARTICLE_SIZE, 256) / 256, 1, 1);

            ZeroMemory(uav, sizeof(uav));
            STContext()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uav),
                uav, nullptr);
        }

        {
            STContext()->CSSetShader(mResetParticlesShader,
                nullptr, 0);
            ID3D11UnorderedAccessView* uav[] = { mPartA_Uav,mPartB_Uav };
            UINT initialCount[] = { (UINT)-1,(UINT)-1 };
            STContext()->CSSetUnorderedAccessViews(0,
                ARRAYSIZE(uav), uav, initialCount);

            STContext()->Dispatch(
                Tool::Align(PTC_MAX_PARTICLE_SIZE, 256) / 256, 1, 1);

            ZeroMemory(uav, sizeof(uav));
            STContext()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uav),
                uav, nullptr);
        }

        mRSParticleContainerPtr->FinishResetRSParticleSystem();
    }

    {
        STContext()->CSSetShader(mEmitParticleShader, nullptr, 0);
        ID3D11UnorderedAccessView* uav[] =
        { mPartA_Uav,mPartB_Uav,mDeadList_Uav };
        ID3D11ShaderResourceView* srv[] = { mRandomTex_Srv };
        ID3D11Buffer* cbuffer[] =
        { mEmitterConstantBuffer,mDeadListConstantBuffer,mTimeConstantBuffer };
        ID3D11SamplerState* sam[] = { mLinearWrapSampler };
        UINT initialCount[] = { (UINT)-1,(UINT)-1,(UINT)-1 };
        STContext()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uav),
            uav, initialCount);
        STContext()->CSSetShaderResources(0, ARRAYSIZE(srv), srv);
        STContext()->CSSetConstantBuffers(0, ARRAYSIZE(cbuffer), cbuffer);
        STContext()->CSSetSamplers(0, ARRAYSIZE(sam), sam);

        auto emitters = mRSParticleContainerPtr->
            GetAllParticleEmitters();
        D3D11_MAPPED_SUBRESOURCE msr = {};
        STContext()->Map(mTimeConstantBuffer, 0,
            D3D11_MAP_WRITE_DISCARD, 0, &msr);
        PTC_TIME_CONSTANT* time = (PTC_TIME_CONSTANT*)msr.pData;
        static float timer = 0.f;
        time->mDeltaTime = 0.016f;
        timer += 0.016f;
        time->mTotalTime = timer;
        STContext()->Unmap(mTimeConstantBuffer, 0);
        for (auto& emitter : *emitters)
        {
            auto& rsinfo = emitter->GetRSParticleEmitterInfo();
            rsinfo.mAccumulation += rsinfo.mEmitNumPerSecond *
                0.016666667f;
            if (rsinfo.mAccumulation > 1.f)
            {
                float integerPart = 0.0f;
                float fraction = modf(rsinfo.mAccumulation,
                    &integerPart);
                rsinfo.mNumToEmit = (int)integerPart;
                rsinfo.mAccumulation = fraction;
            }

            STContext()->Map(mEmitterConstantBuffer, 0,
                D3D11_MAP_WRITE_DISCARD, 0, &msr);
            RS_PARTICLE_EMITTER_INFO* emitterCon =
                (RS_PARTICLE_EMITTER_INFO*)msr.pData;
            emitterCon->mEmitterIndex = rsinfo.mEmitterIndex;
            emitterCon->mEmitNumPerSecond = rsinfo.mEmitNumPerSecond;
            emitterCon->mNumToEmit = rsinfo.mNumToEmit;
            emitterCon->mAccumulation = rsinfo.mAccumulation;
            emitterCon->mPosition = rsinfo.mPosition;
            emitterCon->mVelocity = rsinfo.mVelocity;
            emitterCon->mPosVariance = rsinfo.mPosVariance;
            emitterCon->mVelVariance = rsinfo.mVelVariance;
            emitterCon->mAcceleration = rsinfo.mAcceleration;
            emitterCon->mParticleMass = rsinfo.mParticleMass;
            emitterCon->mLifeSpan = rsinfo.mLifeSpan;
            emitterCon->mOffsetStartSize = rsinfo.mOffsetStartSize;
            emitterCon->mOffsetEndSize = rsinfo.mOffsetEndSize;
            emitterCon->mOffsetStartColor = rsinfo.mOffsetStartColor;
            emitterCon->mOffsetEndColor = rsinfo.mOffsetEndColor;
            emitterCon->mTextureID = rsinfo.mTextureID;
            emitterCon->mStreakFlg = rsinfo.mStreakFlg;
            emitterCon->mMiscFlg = rsinfo.mMiscFlg;
            STContext()->Unmap(mEmitterConstantBuffer, 0);
            STContext()->CopyStructureCount(mDeadListConstantBuffer,
                0, mDeadList_Uav);

            int threadGroupNum = Tool::Align(
                rsinfo.mNumToEmit, 1024) / 1024;
            STContext()->Dispatch(threadGroupNum, 1, 1);
        }

        ZeroMemory(uav, sizeof(uav));
        STContext()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uav), uav, nullptr);
        ZeroMemory(srv, sizeof(srv));
        STContext()->CSSetShaderResources(0, ARRAYSIZE(srv), srv);
    }

    {
        D3D11_MAPPED_SUBRESOURCE msr = {};
        DirectX::XMMATRIX mat = {};

        STContext()->Map(mCameraConstantBuffer, 0,
            D3D11_MAP_WRITE_DISCARD, 0, &msr);
        CAMERA_STATUS* camStatus =
            (CAMERA_STATUS*)msr.pData;
        mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mViewMat);
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&(camStatus->mView), mat);
        mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mInvViewMat);
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&(camStatus->mInvView), mat);
        mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mProjMat);
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&(camStatus->mProj), mat);
        mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mInvProjMat);
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&(camStatus->mInvProj), mat);
        mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mViewProjMat);
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&(camStatus->mViewProj), mat);
        camStatus->mEyePosition = mRSCameraInfo->mEyePosition;
        STContext()->Unmap(mCameraConstantBuffer, 0);

        static auto emitterVec = mRSParticleContainerPtr->
            GetAllParticleEmitters();
        auto size = emitterVec->size();
        STContext()->Map(mSimulEmitterStructedBuffer, 0,
            D3D11_MAP_WRITE_DISCARD, 0, &msr);
        SIMULATE_EMITTER_INFO* emitter =
            (SIMULATE_EMITTER_INFO*)msr.pData;
        for (size_t i = 0; i < size; i++)
        {
            emitter[i].mWorldPosition = (*emitterVec)[i]->
                GetRSParticleEmitterInfo().mPosition;
        }
        STContext()->Unmap(mSimulEmitterStructedBuffer, 0);

        ID3D11Buffer* cb[] = { mCameraConstantBuffer };
        ID3D11ShaderResourceView* srv[] =
        { mDepthTex_Srv,mSimulEmitterStructedBuffer_Srv };
        ID3D11UnorderedAccessView* uav[] =
        { mPartA_Uav,mPartB_Uav,mDeadList_Uav,mAliveIndex_Uav,
        mViewSpacePos_Uav,mMaxRadius_Uav };
        UINT initialCount[] =
        { (UINT)-1,(UINT)-1,(UINT)-1,0,(UINT)-1,(UINT)-1 };

        STContext()->CSSetShader(mSimulateShader, nullptr, 0);
        STContext()->CSSetConstantBuffers(0, ARRAYSIZE(cb), cb);
        STContext()->CSSetShaderResources(0, ARRAYSIZE(srv), srv);
        STContext()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uav),
            uav, initialCount);
        static int threadGroupNum = Tool::Align(
            PTC_MAX_PARTICLE_SIZE, 256) / 256;
        STContext()->Dispatch(threadGroupNum, 1, 1);

        ZeroMemory(uav, sizeof(uav));
        STContext()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uav), uav, nullptr);
        ZeroMemory(srv, sizeof(srv));
        STContext()->CSSetShaderResources(0, ARRAYSIZE(srv), srv);
    }
}

bool RSPass_PriticleEmitSimulate::CreateSampler()
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

    hr = Device()->CreateSamplerState(&sampDesc, &mLinearWrapSampler);
    if (FAILED(hr)) { return false; }

    return true;
}

bool RSPass_PriticleEmitSimulate::CreateShaders()
{
    ID3DBlob* shaderBlob = nullptr;
    HRESULT hr = S_OK;

    hr = Tool::CompileShaderFromFile(L".\\Shaders\\ptc_init_compute.hlsl",
        "Main", "cs_5_0", &shaderBlob);
    if (FAILED(hr)) { return false; }

    hr = Device()->CreateComputeShader(shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(), nullptr, &mInitDeadListShader);
    RS_RELEASE(shaderBlob);
    if (FAILED(hr)) { return false; }

    hr = Tool::CompileShaderFromFile(L".\\Shaders\\ptc_reset_compute.hlsl",
        "Main", "cs_5_0", &shaderBlob);
    if (FAILED(hr)) { return false; }

    hr = Device()->CreateComputeShader(shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(), nullptr, &mResetParticlesShader);
    RS_RELEASE(shaderBlob);
    if (FAILED(hr)) { return false; }

    hr = Tool::CompileShaderFromFile(L".\\Shaders\\ptc_emit_compute.hlsl",
        "Main", "cs_5_0", &shaderBlob);
    if (FAILED(hr)) { return false; }

    hr = Device()->CreateComputeShader(shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(), nullptr, &mEmitParticleShader);
    RS_RELEASE(shaderBlob);
    if (FAILED(hr)) { return false; }

    hr = Tool::CompileShaderFromFile(L".\\Shaders\\ptc_simulate_compute.hlsl",
        "Main", "cs_5_0", &shaderBlob);
    if (FAILED(hr)) { return false; }

    hr = Device()->CreateComputeShader(shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(), nullptr, &mSimulateShader);
    RS_RELEASE(shaderBlob);
    if (FAILED(hr)) { return false; }

    return true;
}

bool RSPass_PriticleEmitSimulate::CheckResources()
{
    auto resManager = GetRSRoot_DX11_Singleton()->ResourceManager();
    if (!resManager) { return false; }

    std::string name = PTC_DEAD_LIST_NAME;
    mDeadList_Uav = resManager->GetResourceInfo(name)->mUav;
    if (!mDeadList_Uav) { return false; }

    name = PTC_A_NAME;
    mPartA_Uav = resManager->GetResourceInfo(name)->mUav;
    if (!mPartA_Uav) { return false; }

    name = PTC_B_NAME;
    mPartB_Uav = resManager->GetResourceInfo(name)->mUav;
    if (!mPartB_Uav) { return false; }

    name = PTC_RAMDOM_TEXTURE_NAME;
    mRandomTex_Srv = resManager->GetResourceInfo(name)->mSrv;
    if (!mRandomTex_Srv) { return false; }

    name = PTC_EMITTER_CONSTANT_NAME;
    mEmitterConstantBuffer = resManager->GetResourceInfo(name)->
        mResource.mBuffer;
    if (!mEmitterConstantBuffer) { return false; }

    name = PTC_DEAD_LIST_CONSTANT_NAME;
    mDeadListConstantBuffer = resManager->GetResourceInfo(name)->
        mResource.mBuffer;
    if (!mDeadListConstantBuffer) { return false; }

    name = PTC_CAMERA_CONSTANT_NAME;
    mCameraConstantBuffer = resManager->GetResourceInfo(name)->
        mResource.mBuffer;
    if (!mCameraConstantBuffer) { return false; }

    name = PTC_SIMU_EMITTER_STRU_NAME;
    mSimulEmitterStructedBuffer_Srv = resManager->GetResourceInfo(name)->mSrv;
    mSimulEmitterStructedBuffer = resManager->GetResourceInfo(name)->
        mResource.mBuffer;
    if (!mSimulEmitterStructedBuffer_Srv || !mSimulEmitterStructedBuffer) { return false; }

    name = PTC_ALIVE_INDEX_NAME;
    mAliveIndex_Uav = resManager->GetResourceInfo(name)->mUav;
    if (!mAliveIndex_Uav) { return false; }

    name = PTC_VIEW_SPCACE_POS_NAME;
    mViewSpacePos_Uav = resManager->GetResourceInfo(name)->mUav;
    if (!mViewSpacePos_Uav) { return false; }

    name = PTC_MAX_RADIUS_NAME;
    mMaxRadius_Uav = resManager->GetResourceInfo(name)->mUav;
    if (!mMaxRadius_Uav) { return false; }

    name = PTC_TIME_CONSTANT_NAME;
    mTimeConstantBuffer = resManager->GetResourceInfo(name)->
        mResource.mBuffer;
    if (!mTimeConstantBuffer) { return false; }

    name = "mrt-depth";
    mDepthTex_Srv = resManager->GetResourceInfo(name)->mSrv;
    if (!mDepthTex_Srv) { return false; }

    return true;
}

RSPass_PriticleTileRender::RSPass_PriticleTileRender(
    std::string& _name, PASS_TYPE _type, RSRoot_DX11* _root) :
    RSPass_Base(_name, _type, _root),
    mCoarseCullingShader(nullptr), mTileCullingShader(nullptr),
    mTileRenderShader(nullptr), mAliveIndex_Uav(nullptr),
    mCameraConstantBuffer(nullptr), mTilingConstantBuffer(nullptr),
    mActiveListConstantBuffer(nullptr), mDepthTex_Srv(nullptr),
    mViewSpacePos_Srv(nullptr), mMaxRadius_Srv(nullptr),
    mAliveIndex_Srv(nullptr), mPartA_Srv(nullptr),
    mCoarseTileIndex_Srv(nullptr), mCoarseTileIndex_Uav(nullptr),
    mCoarseTileIndexCounter_Srv(nullptr), mCoarseTileIndexCounter_Uav(nullptr),
    mTiledIndex_Srv(nullptr), mTiledIndex_Uav(nullptr),
    mParticleRender_Srv(nullptr), mParticleRender_Uav(nullptr),
    mLinearClampSampler(nullptr), mParticleTex_Srv(nullptr),
    mRSCameraInfo(nullptr), mParticleBlendState(nullptr),
    mBlendVertexShader(nullptr), mBlendPixelShader(nullptr)
{

}

RSPass_PriticleTileRender::RSPass_PriticleTileRender(
    const RSPass_PriticleTileRender& _source) :
    RSPass_Base(_source),
    mCoarseCullingShader(_source.mCoarseCullingShader),
    mTileCullingShader(_source.mTileCullingShader),
    mTileRenderShader(_source.mTileRenderShader),
    mCameraConstantBuffer(_source.mCameraConstantBuffer),
    mTilingConstantBuffer(_source.mTilingConstantBuffer),
    mActiveListConstantBuffer(_source.mActiveListConstantBuffer),
    mDepthTex_Srv(_source.mDepthTex_Srv),
    mViewSpacePos_Srv(_source.mViewSpacePos_Srv),
    mMaxRadius_Srv(_source.mMaxRadius_Srv),
    mAliveIndex_Srv(_source.mAliveIndex_Srv),
    mPartA_Srv(_source.mPartA_Srv),
    mCoarseTileIndex_Srv(_source.mCoarseTileIndex_Srv),
    mCoarseTileIndex_Uav(_source.mCoarseTileIndex_Uav),
    mCoarseTileIndexCounter_Srv(_source.mCoarseTileIndexCounter_Srv),
    mCoarseTileIndexCounter_Uav(_source.mCoarseTileIndexCounter_Uav),
    mTiledIndex_Srv(_source.mTiledIndex_Srv),
    mTiledIndex_Uav(_source.mTiledIndex_Uav),
    mParticleRender_Srv(_source.mParticleRender_Srv),
    mParticleRender_Uav(_source.mParticleRender_Uav),
    mLinearClampSampler(_source.mLinearClampSampler),
    mParticleTex_Srv(_source.mParticleTex_Srv),
    mAliveIndex_Uav(_source.mAliveIndex_Uav),
    mRSCameraInfo(_source.mRSCameraInfo),
    mBlendVertexShader(_source.mBlendVertexShader),
    mBlendPixelShader(_source.mBlendPixelShader),
    mParticleBlendState(_source.mParticleBlendState)
{

}

RSPass_PriticleTileRender::~RSPass_PriticleTileRender()
{

}

RSPass_PriticleTileRender* RSPass_PriticleTileRender::ClonePass()
{
    return new RSPass_PriticleTileRender(*this);
}

bool RSPass_PriticleTileRender::InitPass()
{
    std::string name = "temp-cam";
    mRSCameraInfo = GetRSRoot_DX11_Singleton()->CamerasContainer()->
        GetRSCameraInfo(name);
    if (!mRSCameraInfo) { return false; }

    if (!CreateShaders()) { return false; }
    if (!CreateViews()) { return false; }
    if (!CreateSampler()) { return false; }
    if (!CreateBlend()) { return false; }
    if (!CheckResources()) { return false; }

    return true;
}

void RSPass_PriticleTileRender::ReleasePass()
{
    RS_RELEASE(mCoarseCullingShader);
    RS_RELEASE(mTileCullingShader);
    RS_RELEASE(mTileRenderShader);
    RS_RELEASE(mBlendVertexShader);
    RS_RELEASE(mBlendPixelShader);
    RS_RELEASE(mParticleBlendState);

    RS_RELEASE(mLinearClampSampler);

    RS_RELEASE(mParticleTex_Srv);
}

void RSPass_PriticleTileRender::ExecuatePass()
{
    {
        D3D11_MAPPED_SUBRESOURCE msr = {};
        DirectX::XMMATRIX mat = {};
        STContext()->Map(mCameraConstantBuffer, 0,
            D3D11_MAP_WRITE_DISCARD, 0, &msr);
        CAMERA_STATUS* camStatus = (CAMERA_STATUS*)msr.pData;
        mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mViewMat);
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&(camStatus->mView), mat);
        mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mInvViewMat);
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&(camStatus->mInvView), mat);
        mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mProjMat);
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&(camStatus->mProj), mat);
        mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mInvProjMat);
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&(camStatus->mInvProj), mat);
        mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mViewProjMat);
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&(camStatus->mViewProj), mat);
        camStatus->mEyePosition = mRSCameraInfo->mEyePosition;
        STContext()->Unmap(mCameraConstantBuffer, 0);
        STContext()->Map(mTilingConstantBuffer, 0,
            D3D11_MAP_WRITE_DISCARD, 0, &msr);
        RS_TILING_CONSTANT* tiling = (RS_TILING_CONSTANT*)msr.pData;
        tiling->mNumTilesX = g_ParticleSetUpPass->
            GetTilingConstantInfo().mNumTilesX;
        tiling->mNumTilesY = g_ParticleSetUpPass->
            GetTilingConstantInfo().mNumTilesY;
        tiling->mNumCoarseCullingTilesX = g_ParticleSetUpPass->
            GetTilingConstantInfo().mNumCoarseCullingTilesX;
        tiling->mNumCoarseCullingTilesY = g_ParticleSetUpPass->
            GetTilingConstantInfo().mNumCoarseCullingTilesY;
        tiling->mNumCullingTilesPerCoarseTileX = g_ParticleSetUpPass->
            GetTilingConstantInfo().mNumCullingTilesPerCoarseTileX;
        tiling->mNumCullingTilesPerCoarseTileY = g_ParticleSetUpPass->
            GetTilingConstantInfo().mNumCullingTilesPerCoarseTileY;
        STContext()->Unmap(mTilingConstantBuffer, 0);
        STContext()->CopyStructureCount(mActiveListConstantBuffer, 0,
            mAliveIndex_Uav);

        ID3D11Buffer* cb[] =
        { mCameraConstantBuffer,mTilingConstantBuffer,mActiveListConstantBuffer };
        ID3D11ShaderResourceView* srv[] =
        { mViewSpacePos_Srv,mMaxRadius_Srv,mAliveIndex_Srv };
        ID3D11UnorderedAccessView* uav[] =
        { mCoarseTileIndex_Uav,mCoarseTileIndexCounter_Uav };
        UINT initial[] = { (UINT)-1,(UINT)-1 };

        STContext()->CSSetShader(mCoarseCullingShader, nullptr, 0);
        STContext()->CSSetConstantBuffers(0, ARRAYSIZE(cb), cb);
        STContext()->CSSetShaderResources(0, ARRAYSIZE(srv), srv);
        STContext()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uav), uav, initial);

        static int threadGroupNum = Tool::Align(PTC_MAX_PARTICLE_SIZE,
            PTC_COARSE_CULLING_THREADS) / PTC_COARSE_CULLING_THREADS;
        STContext()->Dispatch(threadGroupNum, 1, 1);

        ZeroMemory(cb, sizeof(cb));
        ZeroMemory(srv, sizeof(srv));
        ZeroMemory(uav, sizeof(uav));
        STContext()->CSSetConstantBuffers(0, ARRAYSIZE(cb), cb);
        STContext()->CSSetShaderResources(0, ARRAYSIZE(srv), srv);
        STContext()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uav), uav, nullptr);
    }

    {
        D3D11_MAPPED_SUBRESOURCE msr = {};
        DirectX::XMMATRIX mat = {};
        STContext()->Map(mCameraConstantBuffer, 0,
            D3D11_MAP_WRITE_DISCARD, 0, &msr);
        CAMERA_STATUS* camStatus = (CAMERA_STATUS*)msr.pData;
        mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mViewMat);
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&(camStatus->mView), mat);
        mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mInvViewMat);
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&(camStatus->mInvView), mat);
        mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mProjMat);
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&(camStatus->mProj), mat);
        mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mInvProjMat);
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&(camStatus->mInvProj), mat);
        mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mViewProjMat);
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&(camStatus->mViewProj), mat);
        camStatus->mEyePosition = mRSCameraInfo->mEyePosition;
        STContext()->Unmap(mCameraConstantBuffer, 0);
        STContext()->Map(mTilingConstantBuffer, 0,
            D3D11_MAP_WRITE_DISCARD, 0, &msr);
        RS_TILING_CONSTANT* tiling = (RS_TILING_CONSTANT*)msr.pData;
        tiling->mNumTilesX = g_ParticleSetUpPass->
            GetTilingConstantInfo().mNumTilesX;
        tiling->mNumTilesY = g_ParticleSetUpPass->
            GetTilingConstantInfo().mNumTilesY;
        tiling->mNumCoarseCullingTilesX = g_ParticleSetUpPass->
            GetTilingConstantInfo().mNumCoarseCullingTilesX;
        tiling->mNumCoarseCullingTilesY = g_ParticleSetUpPass->
            GetTilingConstantInfo().mNumCoarseCullingTilesY;
        tiling->mNumCullingTilesPerCoarseTileX = g_ParticleSetUpPass->
            GetTilingConstantInfo().mNumCullingTilesPerCoarseTileX;
        tiling->mNumCullingTilesPerCoarseTileY = g_ParticleSetUpPass->
            GetTilingConstantInfo().mNumCullingTilesPerCoarseTileY;
        STContext()->Unmap(mTilingConstantBuffer, 0);

        ID3D11Buffer* cb[] =
        { mCameraConstantBuffer,mTilingConstantBuffer,mActiveListConstantBuffer };
        ID3D11ShaderResourceView* srv[] =
        { mViewSpacePos_Srv,mMaxRadius_Srv,mAliveIndex_Srv,
        mDepthTex_Srv,mCoarseTileIndex_Srv,mCoarseTileIndexCounter_Srv };
        ID3D11UnorderedAccessView* uav[] = { mTiledIndex_Uav };
        UINT initial[] = { (UINT)-1 };

        STContext()->CSSetShader(mTileCullingShader, nullptr, 0);
        STContext()->CSSetConstantBuffers(0, ARRAYSIZE(cb), cb);
        STContext()->CSSetShaderResources(0, ARRAYSIZE(srv), srv);
        STContext()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uav), uav, initial);

        STContext()->Dispatch(
            g_ParticleSetUpPass->GetTilingConstantInfo().mNumTilesX,
            g_ParticleSetUpPass->GetTilingConstantInfo().mNumTilesY, 1);

        ZeroMemory(cb, sizeof(cb));
        ZeroMemory(srv, sizeof(srv));
        ZeroMemory(uav, sizeof(uav));
        STContext()->CSSetConstantBuffers(0, ARRAYSIZE(cb), cb);
        STContext()->CSSetShaderResources(0, ARRAYSIZE(srv), srv);
        STContext()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uav), uav, nullptr);
    }

    {
        D3D11_MAPPED_SUBRESOURCE msr = {};
        DirectX::XMMATRIX mat = {};
        STContext()->Map(mCameraConstantBuffer, 0,
            D3D11_MAP_WRITE_DISCARD, 0, &msr);
        CAMERA_STATUS* camStatus = (CAMERA_STATUS*)msr.pData;
        mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mViewMat);
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&(camStatus->mView), mat);
        mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mInvViewMat);
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&(camStatus->mInvView), mat);
        mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mProjMat);
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&(camStatus->mProj), mat);
        mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mInvProjMat);
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&(camStatus->mInvProj), mat);
        mat = DirectX::XMLoadFloat4x4(&mRSCameraInfo->mViewProjMat);
        mat = DirectX::XMMatrixTranspose(mat);
        DirectX::XMStoreFloat4x4(&(camStatus->mViewProj), mat);
        camStatus->mEyePosition = mRSCameraInfo->mEyePosition;
        STContext()->Unmap(mCameraConstantBuffer, 0);
        STContext()->Map(mTilingConstantBuffer, 0,
            D3D11_MAP_WRITE_DISCARD, 0, &msr);
        RS_TILING_CONSTANT* tiling = (RS_TILING_CONSTANT*)msr.pData;
        tiling->mNumTilesX = g_ParticleSetUpPass->
            GetTilingConstantInfo().mNumTilesX;
        tiling->mNumTilesY = g_ParticleSetUpPass->
            GetTilingConstantInfo().mNumTilesY;
        tiling->mNumCoarseCullingTilesX = g_ParticleSetUpPass->
            GetTilingConstantInfo().mNumCoarseCullingTilesX;
        tiling->mNumCoarseCullingTilesY = g_ParticleSetUpPass->
            GetTilingConstantInfo().mNumCoarseCullingTilesY;
        tiling->mNumCullingTilesPerCoarseTileX = g_ParticleSetUpPass->
            GetTilingConstantInfo().mNumCullingTilesPerCoarseTileX;
        tiling->mNumCullingTilesPerCoarseTileY = g_ParticleSetUpPass->
            GetTilingConstantInfo().mNumCullingTilesPerCoarseTileY;
        STContext()->Unmap(mTilingConstantBuffer, 0);

        ID3D11Buffer* cb[] =
        { mCameraConstantBuffer,mTilingConstantBuffer };
        ID3D11ShaderResourceView* srv[] =
        { mPartA_Srv,mViewSpacePos_Srv,mDepthTex_Srv,mTiledIndex_Srv,
        mCoarseTileIndexCounter_Srv,mParticleTex_Srv };
        ID3D11UnorderedAccessView* uav[] = { mParticleRender_Uav };
        UINT initial[] = { (UINT)-1 };
        ID3D11SamplerState* sam[] = { mLinearClampSampler };

        STContext()->CSSetShader(mTileRenderShader, nullptr, 0);
        STContext()->CSSetConstantBuffers(0, ARRAYSIZE(cb), cb);
        STContext()->CSSetShaderResources(0, ARRAYSIZE(srv), srv);
        STContext()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uav), uav, initial);
        STContext()->CSSetSamplers(0, ARRAYSIZE(sam), sam);

        STContext()->Dispatch(
            g_ParticleSetUpPass->GetTilingConstantInfo().mNumTilesX,
            g_ParticleSetUpPass->GetTilingConstantInfo().mNumTilesY, 1);

        ZeroMemory(cb, sizeof(cb));
        ZeroMemory(srv, sizeof(srv));
        ZeroMemory(uav, sizeof(uav));
        STContext()->CSSetConstantBuffers(0, ARRAYSIZE(cb), cb);
        STContext()->CSSetShaderResources(0, ARRAYSIZE(srv), srv);
        STContext()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uav), uav, nullptr);
        STContext()->CSSetShader(nullptr, nullptr, 0);
    }

    {
        static auto rtv = GetRSRoot_DX11_Singleton()->Devices()->GetSwapChainRtv();
        static D3D11_VIEWPORT vp = {};
        vp.Width = 1280.f; vp.Height = 720.f; vp.MinDepth = 0.f;
        vp.MaxDepth = 1.f; vp.TopLeftX = 0.f; vp.TopLeftY = 0.f;
        STContext()->VSSetShader(mBlendVertexShader, nullptr, 0);
        STContext()->PSSetShader(mBlendPixelShader, nullptr, 0);
        STContext()->OMSetBlendState(mParticleBlendState, nullptr, 0xFFFFFFFF);
        STContext()->OMSetRenderTargets(1, &rtv, nullptr);
        STContext()->RSSetViewports(1, &vp);
        STContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
        STContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        ID3D11ShaderResourceView* srv[] = { mParticleRender_Srv };
        STContext()->PSSetShaderResources(0, ARRAYSIZE(srv), srv);

        STContext()->Draw(3, 0);

        ZeroMemory(srv, sizeof(srv));
        STContext()->PSSetShaderResources(0, ARRAYSIZE(srv), srv);
        STContext()->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
    }
}

bool RSPass_PriticleTileRender::CreateShaders()
{
    ID3DBlob* shaderBlob = nullptr;
    HRESULT hr = S_OK;

    hr = Tool::CompileShaderFromFile(L".\\Shaders\\ptc_coarse_compute.hlsl",
        "Main", "cs_5_0", &shaderBlob);
    if (FAILED(hr)) { return false; }

    hr = Device()->CreateComputeShader(shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(), nullptr, &mCoarseCullingShader);
    RS_RELEASE(shaderBlob);
    if (FAILED(hr)) { return false; }

    hr = Tool::CompileShaderFromFile(L".\\Shaders\\ptc_cull_compute.hlsl",
        "Main", "cs_5_0", &shaderBlob);
    if (FAILED(hr)) { return false; }

    hr = Device()->CreateComputeShader(shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(), nullptr, &mTileCullingShader);
    RS_RELEASE(shaderBlob);
    if (FAILED(hr)) { return false; }

    hr = Tool::CompileShaderFromFile(L".\\Shaders\\ptc_render_compute.hlsl",
        "Main", "cs_5_0", &shaderBlob);
    if (FAILED(hr)) { return false; }

    hr = Device()->CreateComputeShader(shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(), nullptr, &mTileRenderShader);
    RS_RELEASE(shaderBlob);
    if (FAILED(hr)) { return false; }

    hr = Tool::CompileShaderFromFile(L".\\Shaders\\ptc_blend_vertex.hlsl",
        "Main", "vs_5_0", &shaderBlob);
    if (FAILED(hr)) { return false; }

    hr = Device()->CreateVertexShader(shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(), nullptr, &mBlendVertexShader);
    RS_RELEASE(shaderBlob);
    if (FAILED(hr)) { return false; }

    hr = Tool::CompileShaderFromFile(L".\\Shaders\\ptc_blend_pixel.hlsl",
        "Main", "ps_5_0", &shaderBlob);
    if (FAILED(hr)) { return false; }

    hr = Device()->CreatePixelShader(shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(), nullptr, &mBlendPixelShader);
    RS_RELEASE(shaderBlob);
    if (FAILED(hr)) { return false; }

    return true;
}

bool RSPass_PriticleTileRender::CreateViews()
{
    HRESULT hr = S_OK;

    std::wstring path = L".\\Textures\\particle_atlas.dds";
    hr = DirectX::CreateDDSTextureFromFile(Device(), path.c_str(),
        nullptr, &mParticleTex_Srv);
    if (FAILED(hr)) { return false; }

    return true;
}

bool RSPass_PriticleTileRender::CreateSampler()
{
    HRESULT hr = S_OK;
    D3D11_SAMPLER_DESC sampDesc = {};
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    hr = Device()->CreateSamplerState(&sampDesc, &mLinearClampSampler);
    if (FAILED(hr)) { return false; }

    return true;
}

bool RSPass_PriticleTileRender::CreateBlend()
{
    HRESULT hr = S_OK;

    D3D11_BLEND_DESC bldDesc = {};
    ZeroMemory(&bldDesc, sizeof(D3D11_BLEND_DESC));
    bldDesc.AlphaToCoverageEnable = false;
    bldDesc.IndependentBlendEnable = false;
    bldDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    bldDesc.RenderTarget[0].BlendEnable = true;
    bldDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    bldDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    bldDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    bldDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
    bldDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    bldDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    hr = Device()->CreateBlendState(&bldDesc, &mParticleBlendState);
    if (FAILED(hr)) { return false; }

    return true;
}

bool RSPass_PriticleTileRender::CheckResources()
{
    auto resManager = GetRSRoot_DX11_Singleton()->ResourceManager();
    if (!resManager) { return false; }

    std::string name = PTC_CAMERA_CONSTANT_NAME;
    mCameraConstantBuffer = resManager->GetResourceInfo(name)->
        mResource.mBuffer;
    if (!mCameraConstantBuffer) { return false; }

    name = PTC_TILING_CONSTANT_NAME;
    mTilingConstantBuffer = resManager->GetResourceInfo(name)->
        mResource.mBuffer;
    if (!mTilingConstantBuffer) { return false; }

    name = PTC_ALIVE_LIST_CONSTANT_NAME;
    mActiveListConstantBuffer = resManager->GetResourceInfo(name)->
        mResource.mBuffer;
    if (!mActiveListConstantBuffer) { return false; }

    name = "mrt-depth";
    mDepthTex_Srv = resManager->GetResourceInfo(name)->mSrv;
    if (!mDepthTex_Srv) { return false; }

    name = PTC_VIEW_SPCACE_POS_NAME;
    mViewSpacePos_Srv = resManager->GetResourceInfo(name)->mSrv;
    if (!mViewSpacePos_Srv) { return false; }

    name = PTC_MAX_RADIUS_NAME;
    mMaxRadius_Srv = resManager->GetResourceInfo(name)->mSrv;
    if (!mMaxRadius_Srv) { return false; }

    name = PTC_ALIVE_INDEX_NAME;
    mAliveIndex_Srv = resManager->GetResourceInfo(name)->mSrv;
    mAliveIndex_Uav = resManager->GetResourceInfo(name)->mUav;
    if (!mAliveIndex_Srv) { return false; }
    if (!mAliveIndex_Uav) { return false; }

    name = PTC_A_NAME;
    mPartA_Srv = resManager->GetResourceInfo(name)->mSrv;
    if (!mPartA_Srv) { return false; }

    name = PTC_COARSE_CULL_NAME;
    mCoarseTileIndex_Srv = resManager->GetResourceInfo(name)->mSrv;
    mCoarseTileIndex_Uav = resManager->GetResourceInfo(name)->mUav;
    if (!mCoarseTileIndex_Srv) { return false; }
    if (!mCoarseTileIndex_Uav) { return false; }

    name = PTC_COARSE_CULL_COUNTER_NAME;
    mCoarseTileIndexCounter_Srv = resManager->GetResourceInfo(name)->mSrv;
    mCoarseTileIndexCounter_Uav = resManager->GetResourceInfo(name)->mUav;
    if (!mCoarseTileIndexCounter_Srv) { return false; }
    if (!mCoarseTileIndexCounter_Uav) { return false; }

    name = PTC_TILED_INDEX_NAME;
    mTiledIndex_Srv = resManager->GetResourceInfo(name)->mSrv;
    mTiledIndex_Uav = resManager->GetResourceInfo(name)->mUav;
    if (!mTiledIndex_Srv) { return false; }
    if (!mTiledIndex_Uav) { return false; }

    name = PTC_RENDER_BUFFER_NAME;
    mParticleRender_Srv = resManager->GetResourceInfo(name)->mSrv;
    mParticleRender_Uav = resManager->GetResourceInfo(name)->mUav;
    if (!mParticleRender_Srv) { return false; }
    if (!mParticleRender_Uav) { return false; }

    return true;
}
