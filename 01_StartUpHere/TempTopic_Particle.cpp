#include "TempTopic_Particle.h"
#include "RSRoot_DX11.h"
#include "RSResourceManager.h"
#include "RSUtilityFunctions.h"
#include "RSShaderCompile.h"
#include "RSParticlesContainer.h"

#define RS_RELEASE(p) { if (p) { (p)->Release(); (p)=nullptr; } }

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
    mSimulEmitterStructedBuffer_Srv(nullptr)
{

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
    mSimulEmitterStructedBuffer_Srv(_source.mSimulEmitterStructedBuffer_Srv)
{

}

RSPass_PriticleSetUp::~RSPass_PriticleSetUp()
{

}

RSPass_PriticleSetUp* RSPass_PriticleSetUp::ClonePass()
{
    return new RSPass_PriticleSetUp(*this);
}

bool RSPass_PriticleSetUp::InitPass()
{
    mTilingConstant.mNumTilesX =
        Tool::Align(1280, PTC_TILE_X_SIZE) / PTC_TILE_X_SIZE;
    mTilingConstant.mNumTilesY =
        Tool::Align(720, PTC_TILE_Y_SIZE) / PTC_TILE_Y_SIZE;
    mTilingConstant.mNumCoarseCullingTilesX = 8;
    mTilingConstant.mNumCoarseCullingTilesY = 8;
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
    mEmitParticleShader(nullptr), mSimulateShader(nullptr)
{

}

RSPass_PriticleEmitSimulate::RSPass_PriticleEmitSimulate(
    const RSPass_PriticleEmitSimulate& _source) :
    RSPass_Base(_source),
    mRSParticleContainerPtr(_source.mRSParticleContainerPtr),
    mInitDeadListShader(_source.mInitDeadListShader),
    mResetParticlesShader(_source.mResetParticlesShader),
    mEmitParticleShader(_source.mEmitParticleShader),
    mSimulateShader(_source.mSimulateShader)
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

    if (!CreateShaders()) { return false; }
    if (!CheckResources()) { return false; }

    return true;
}

void RSPass_PriticleEmitSimulate::ReleasePass()
{

}

void RSPass_PriticleEmitSimulate::ExecuatePass()
{

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
    HRESULT hr = S_OK;

    return true;
}
