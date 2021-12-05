#include "TempTopic_Particle.h"
#include "RSRoot_DX11.h"
#include "RSResourceManager.h"

RSPass_PriticleSetUp::RSPass_PriticleSetUp(
    std::string& _name, PASS_TYPE _type, RSRoot_DX11* _root) :
    RSPass_Base(_name, _type, _root),
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
    mTilingConstantBuffer(nullptr),
    mDebugCounterBuffer(nullptr)
{

}

RSPass_PriticleSetUp::RSPass_PriticleSetUp(
    const RSPass_PriticleSetUp& _source) :
    RSPass_Base(_source),
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
    mTilingConstantBuffer(_source.mTilingConstantBuffer),
    mDebugCounterBuffer(_source.mDebugCounterBuffer)
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
    if (!CreateBuffers()) { return false; }
    if (!CreateViews()) { return false; }

    return true;
}

void RSPass_PriticleSetUp::ReleasePass()
{
    auto root = GetRSRoot_DX11_Singleton();
    std::string name = PTC_A_NAME;
    root->ResourceManager()->DeleteResource(name);
    name = PTC_B_NAME;
    root->ResourceManager()->DeleteResource(name);
    name = PTC_VIEW_SPCACE_POS_NAME;
    root->ResourceManager()->DeleteResource(name);
    name = PTC_MAX_RADIUS_NAME;
    root->ResourceManager()->DeleteResource(name);
    name = PTC_COARSE_CULL_NAME;
    root->ResourceManager()->DeleteResource(name);
    name = PTC_COARSE_CULL_COUNTER_NAME;
    root->ResourceManager()->DeleteResource(name);
    name = PTC_TILED_INDEX_NAME;
    root->ResourceManager()->DeleteResource(name);
    name = PTC_DEAD_LIST_NAME;
    root->ResourceManager()->DeleteResource(name);
    name = PTC_ALIVE_INDEX_NAME;
    root->ResourceManager()->DeleteResource(name);
    name = PTC_DEAD_LIST_CONSTANT_NAME;
    root->ResourceManager()->DeleteResource(name);
    name = PTC_ALIVE_LIST_CONSTANT_NAME;
    root->ResourceManager()->DeleteResource(name);
    name = PTC_EMITTER_CONSTANT_NAME;
    root->ResourceManager()->DeleteResource(name);
    name = PTC_TILING_CONSTANT_NAME;
    root->ResourceManager()->DeleteResource(name);
    name = PTC_DEBUG_COUNTER_NAME;
    root->ResourceManager()->DeleteResource(name);
}

void RSPass_PriticleSetUp::ExecuatePass()
{

}

bool RSPass_PriticleSetUp::CreateBuffers()
{
    // TEMP---------------
    return false;
}

bool RSPass_PriticleSetUp::CreateViews()
{
    // TEMP-----------------
    return false;
}
