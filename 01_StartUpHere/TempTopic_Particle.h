#pragma once

#include <string>
#include <vector>
#include "RSPass_Base.h"

class RSPass_PriticleSimulate :public RSPass_Base
{
public:
    RSPass_PriticleSimulate(std::string& _name, PASS_TYPE _type,
        class RSRoot_DX11* _root);
    RSPass_PriticleSimulate(const RSPass_PriticleSimulate& _source);
    virtual ~RSPass_PriticleSimulate();

public:
    virtual RSPass_PriticleSimulate* ClonePass() override;

    virtual bool InitPass();

    virtual void ReleasePass();

    virtual void ExecuatePass();

private:
    bool CreateShaders();
    bool CreateBuffers();
    bool CreateViews();

private:
    ID3D11Buffer* mParticlePartA;
    ID3D11ShaderResourceView* mPartA_Srv;
    ID3D11UnorderedAccessView* mPartA_Uav;

    ID3D11Buffer* mParticlePartB;
    ID3D11UnorderedAccessView* mPartB_Uav;

    ID3D11Buffer* mViewspacePosBuffer;
    ID3D11ShaderResourceView* mViewSpacePos_Srv;
    ID3D11UnorderedAccessView* mViewSpacePos_Uav;

    ID3D11Buffer* mMaxRadiusBuffer;
    ID3D11ShaderResourceView* mMaxRadius_Srv;
    ID3D11UnorderedAccessView* mMaxRadius_Uav;

    ID3D11Buffer* mStridedCoarseCullBuffer;
    ID3D11ShaderResourceView* mStridedCoarseCull_Srv;
    ID3D11UnorderedAccessView* mStridedCoarseCull_Uav;

    ID3D11Buffer* mStridedCoarseCullCounterBuffer;
    ID3D11ShaderResourceView* mStridedCoarseCullCounter_Srv;
    ID3D11UnorderedAccessView* mStridedCoarseCullCounter_Uav;

    ID3D11Buffer* mDeadListBuffer;
    ID3D11UnorderedAccessView* mDeadList_Uav;

    ID3D11Buffer* mAliveIndexBuffer;
    ID3D11ShaderResourceView* mAliveIndex_Srv;
    ID3D11UnorderedAccessView* mAliveIndex_Uav;

    ID3D11Buffer* mDeadListConstantBuffer;
    ID3D11Buffer* mActiveListConstantBuffer;

    ID3D11Buffer* mEmitterConstantBuffer;
    ID3D11Buffer* mTilingConstantBuffer;
};
