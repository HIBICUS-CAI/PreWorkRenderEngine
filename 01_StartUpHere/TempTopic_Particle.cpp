#include "TempTopic_Particle.h"
#include "RSRoot_DX11.h"

RSPass_PriticleSimulate::RSPass_PriticleSimulate(
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
    mDeadListBuffer(nullptr), mDeadList_Uav(nullptr),
    mAliveIndexBuffer(nullptr),
    mAliveIndex_Srv(nullptr), mAliveIndex_Uav(nullptr),
    mDeadListConstantBuffer(nullptr),
    mActiveListConstantBuffer(nullptr),
    mEmitterConstantBuffer(nullptr), mTilingConstantBuffer(nullptr)
{

}

RSPass_PriticleSimulate::RSPass_PriticleSimulate(
    const RSPass_PriticleSimulate& _source) :
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
    mDeadListBuffer(_source.mDeadListBuffer),
    mDeadList_Uav(_source.mDeadList_Uav),
    mAliveIndexBuffer(_source.mAliveIndexBuffer),
    mAliveIndex_Srv(_source.mAliveIndex_Srv),
    mAliveIndex_Uav(_source.mAliveIndex_Uav),
    mDeadListConstantBuffer(_source.mDeadListConstantBuffer),
    mActiveListConstantBuffer(_source.mActiveListConstantBuffer),
    mEmitterConstantBuffer(_source.mEmitterConstantBuffer),
    mTilingConstantBuffer(_source.mTilingConstantBuffer)
{

}

RSPass_PriticleSimulate::~RSPass_PriticleSimulate()
{

}

RSPass_PriticleSimulate* RSPass_PriticleSimulate::ClonePass()
{
    return new RSPass_PriticleSimulate(*this);
}

bool RSPass_PriticleSimulate::InitPass()
{
    // TEMP-------------
    return false;
}

void RSPass_PriticleSimulate::ReleasePass()
{

}

void RSPass_PriticleSimulate::ExecuatePass()
{

}

bool RSPass_PriticleSimulate::CreateShaders()
{
    // TEMP-----------
    return false;
}

bool RSPass_PriticleSimulate::CreateBuffers()
{
    // TEMP---------------
    return false;
}

bool RSPass_PriticleSimulate::CreateViews()
{
    // TEMP-----------------
    return false;
}
