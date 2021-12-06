#pragma once

#include <string>
#include <vector>
#include "RSPass_Base.h"

constexpr auto PTC_RENDER_BUFFER_NAME = "particle-render-buffer";
constexpr auto PTC_A_NAME = "particle-part-a";
constexpr auto PTC_B_NAME = "particle-part-b";
constexpr auto PTC_VIEW_SPCACE_POS_NAME = "particle-view-space-pos";
constexpr auto PTC_MAX_RADIUS_NAME = "particle-max-radius";
constexpr auto PTC_COARSE_CULL_NAME = "particle-coarse-cull";
constexpr auto PTC_COARSE_CULL_COUNTER_NAME = "particle-coarse-cull-counter";
constexpr auto PTC_TILED_INDEX_NAME = "particle-tiled-index";
constexpr auto PTC_DEAD_LIST_NAME = "particle-dead-list";
constexpr auto PTC_ALIVE_INDEX_NAME = "particle-alive-index";
constexpr auto PTC_DEAD_LIST_CONSTANT_NAME = "particle-dead-list-constant";
constexpr auto PTC_ALIVE_LIST_CONSTANT_NAME = "particle-alive-list-constant";
constexpr auto PTC_EMITTER_CONSTANT_NAME = "particle-emitter-constant";
constexpr auto PTC_TILING_CONSTANT_NAME = "particle-tiling-constant";
constexpr auto PTC_DEBUG_COUNTER_NAME = "particle-debug-counter";
constexpr auto PTC_RAMDOM_TEXTURE_NAME = "particle-ramdom-texture";
constexpr UINT PTC_MAX_PARTICLE_SIZE = 400 * 1024;
constexpr UINT PTC_MAX_COARSE_CULL_TILE_X = 16;
constexpr UINT PTC_MAX_COARSE_CULL_TILE_Y = 8;
constexpr UINT PTC_MAX_COARSE_CULL_TILE_SIZE = PTC_MAX_COARSE_CULL_TILE_X * PTC_MAX_COARSE_CULL_TILE_Y;
constexpr UINT PTC_NUM_PER_TILE = 1023;
constexpr UINT PTC_TILE_BUFFER_SIZE = PTC_NUM_PER_TILE + 1;
constexpr UINT PTC_TILE_X_SIZE = 32;
constexpr UINT PTC_TILE_Y_SIZE = 32;

struct RS_PARTICLE_PART_A
{
    DirectX::XMFLOAT4 mColorAndAlpha = {};
    DirectX::XMFLOAT2 mViewSpaceVelocityXY = {};
    float mEmitterNormalDotLight = 0.f;
    UINT mEmitterProperties = 0;
    float mRotation = 0.f;
    UINT mIsSleeping = 0;
    UINT mCollisionCount = 0;
    float mPads[1] = { 0.f };
};

struct RS_PARTICLE_PART_B
{
    DirectX::XMFLOAT3 mWorldPosition = {};
    float mMass = 0.f;
    DirectX::XMFLOAT3 mWorldSpaceVelocity = {};
    float mLifeSpan = 0.f;
    float mDistanceToEye = 0.f;
    float mAge = 0.f;
    float mStartSize = 0.f;
    float mEndSize = 0.f;
};

struct RS_ALIVE_INDEX_BUFFER_ELEMENT
{
    float mDistance;
    float mIndex;
};

struct RS_TILING_CONSTANT
{
    UINT mNumTilesX = 0;
    UINT mNumTilesY = 0;
    UINT mNumCoarseCullingTilesX = 0;
    UINT mNumCoarseCullingTilesY = 0;
    UINT mNumCullingTilesPerCoarseTileX = 0;
    UINT mNumCullingTilesPerCoarseTileY = 0;
    UINT mPads[2] = { 0 };
};

class RSPass_PriticleSetUp :public RSPass_Base
{
public:
    RSPass_PriticleSetUp(std::string& _name, PASS_TYPE _type,
        class RSRoot_DX11* _root);
    RSPass_PriticleSetUp(const RSPass_PriticleSetUp& _source);
    virtual ~RSPass_PriticleSetUp();

public:
    virtual RSPass_PriticleSetUp* ClonePass() override;

    virtual bool InitPass();

    virtual void ReleasePass();

    virtual void ExecuatePass();

private:
    bool CreateBuffers();
    bool CreateViews();

private:
    RS_TILING_CONSTANT mTilingConstant;

    ID3D11Buffer* mParticleRenderBuffer;
    ID3D11ShaderResourceView* mParticleRender_Srv;
    ID3D11UnorderedAccessView* mParticleRender_Uav;

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

    ID3D11Buffer* mTiledIndexBuffer;
    ID3D11ShaderResourceView* mTiledIndex_Srv;
    ID3D11UnorderedAccessView* mTiledIndex_Uav;

    ID3D11Buffer* mDeadListBuffer;
    ID3D11UnorderedAccessView* mDeadList_Uav;

    ID3D11Buffer* mAliveIndexBuffer;
    ID3D11ShaderResourceView* mAliveIndex_Srv;
    ID3D11UnorderedAccessView* mAliveIndex_Uav;

    ID3D11Buffer* mDeadListConstantBuffer;
    ID3D11Buffer* mActiveListConstantBuffer;

    ID3D11Buffer* mEmitterConstantBuffer;
    ID3D11Buffer* mTilingConstantBuffer;

    ID3D11Buffer* mDebugCounterBuffer;

    ID3D11Texture2D* mParticleRandomTexture;
    ID3D11ShaderResourceView* mParticleRandom_Srv;
};
