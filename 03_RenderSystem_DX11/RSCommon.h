//---------------------------------------------------------------
// File: RSCommon.h
// Proj: RenderSystem_DX11
// Info: 声明RenderSystem_DX11相关的全局类型
// Date: 2021.9.14
// Mail: cai_genkan@outlook.com
// Comt: 在此lib项目内中提供ExtraMacro
//---------------------------------------------------------------

#pragma once

#ifdef _RS_DX11
#include <vector>
#include <string>
#endif // _RS_DX11
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <Windows.h>

enum class PASS_TYPE
{
    RENDER,
    COMPUTE
};

constexpr UINT RS_INVALID_ORDER = 0;

constexpr UINT MAX_STRUCTURED_BUFFER_SIZE = 256;
constexpr UINT MAX_INSTANCE_SIZE = MAX_STRUCTURED_BUFFER_SIZE;
constexpr UINT MAX_LIGHT_SIZE = MAX_STRUCTURED_BUFFER_SIZE;
constexpr UINT MAX_PARTICLE_EMITTER_SIZE = MAX_STRUCTURED_BUFFER_SIZE;
constexpr UINT MAX_SHADOW_SIZE = 4;

enum class LAYOUT_TYPE
{
    NORMAL_COLOR,
    NORMAL_TEX,
    NORMAL_TANGENT_TEX
};

enum class DRAWCALL_TYPE
{
    OPACITY,
    TRANSPARENCY,
    LIGHT,
    COLOR_GEO,
    MIRROR_SELF,
    MIRRO_INSIDE,
    UI_SPRITE,

    MAX
};

enum class LENS_TYPE
{
    PERSPECTIVE,
    ORTHOGRAPHIC
};

struct CAM_INFO
{
    LENS_TYPE mType = LENS_TYPE::PERSPECTIVE;
    DirectX::XMFLOAT3 mPosition = {};
    DirectX::XMFLOAT3 mLookAt = {};
    DirectX::XMFLOAT3 mUpVec = {};
    DirectX::XMFLOAT2 mPFovyAndRatio = {};
    DirectX::XMFLOAT2 mOWidthAndHeight = {};
    DirectX::XMFLOAT2 mNearFarZ = {};
};

struct RS_CAM_INFO
{
    DirectX::XMFLOAT4X4 mViewMat = {};
    DirectX::XMFLOAT4X4 mInvViewMat = {};
    DirectX::XMFLOAT4X4 mProjMat = {};
    DirectX::XMFLOAT4X4 mInvProjMat = {};
    DirectX::XMFLOAT4X4 mViewProjMat = {};
    DirectX::XMFLOAT3 mEyePosition = {};
};

enum class LIGHT_TYPE
{
    DIRECT,
    POINT,
    SPOT
};

struct LIGHT_INFO
{
    LIGHT_TYPE mType = LIGHT_TYPE::DIRECT;
    bool mWithShadow = false;
    DirectX::XMFLOAT3 mStrength = {};
    float mFalloffStart = 0.f;
    DirectX::XMFLOAT3 mDirection = {};
    float mFalloffEnd = 0.f;
    DirectX::XMFLOAT3 mPosition = {};
    float mSpotPower = 0.f;
};

struct RS_LIGHT_INFO
{
    DirectX::XMFLOAT3 mStrength = {};
    float mFalloffStart = 0.f;
    DirectX::XMFLOAT3 mDirection = {};
    float mFalloffEnd = 0.f;
    DirectX::XMFLOAT3 mPosition = {};
    float mSpotPower = 0.f;
};

enum class PARTICLE_TEXTURE
{
    WHITE_CIRCLE,

    PARTICLE_TEXTURE_SIZE
};

struct PARTICLE_EMITTER_INFO
{
    float mEmitNumPerSecond = 0.f;
    DirectX::XMFLOAT3 mPosition = {};
    DirectX::XMFLOAT3 mVelocity = {};
    DirectX::XMFLOAT3 mPosVariance = {};
    float mVelVariance = 0.f;
    DirectX::XMFLOAT3 mAcceleration = {};
    float mParticleMass = 0.f;
    float mLifeSpan = 0.f;
    float mOffsetStartSize = 0.f;
    float mOffsetEndSize = 0.f;
    DirectX::XMFLOAT4 mOffsetStartColor = {};
    DirectX::XMFLOAT4 mOffsetEndColor = {};
    bool mEnableStreak = false;
    PARTICLE_TEXTURE mTextureID = PARTICLE_TEXTURE::WHITE_CIRCLE;
};

struct RS_PARTICLE_EMITTER_INFO
{
    UINT mEmitterIndex = 0;
    float mEmitNumPerSecond = 0.f;
    UINT mNumToEmit = 0;
    float mAccumulation = 0.f;
    alignas(16) DirectX::XMFLOAT3 mPosition = {};
    alignas(16) DirectX::XMFLOAT3 mVelocity = {};
    alignas(16) DirectX::XMFLOAT3 mPosVariance = {};
    alignas(16) DirectX::XMFLOAT3 mAcceleration = {};
    float mVelVariance = 0.f;
    float mParticleMass = 0.f;
    float mLifeSpan = 0.f;
    float mOffsetStartSize = 0.f;
    float mOffsetEndSize = 0.f;
    UINT mTextureID = (UINT)(PARTICLE_TEXTURE::WHITE_CIRCLE);
    UINT mStreakFlg = 0;
    UINT mMiscFlg = 0;
    DirectX::XMFLOAT4 mOffsetStartColor = {};
    DirectX::XMFLOAT4 mOffsetEndColor = {};
};

struct MATERIAL_INFO
{
    DirectX::XMFLOAT4 mDiffuseAlbedo = {};
    DirectX::XMFLOAT3 mFresnelR0 = {};
    float mShininess = 0.f;
};

struct RS_MATERIAL_INFO
{
    DirectX::XMFLOAT4 mDiffuseAlbedo = {};
    DirectX::XMFLOAT3 mFresnelR0 = {};
    float mShininess = 0.f;
};

enum class TOPOLOGY_TYPE
{
    NONE,
    POINTLIST,
    LINELIST,
    LINESTRIP,
    TRIANGLELIST,
    TRIANGLESTRIP
};

struct SUBMESH_INFO
{
    TOPOLOGY_TYPE mTopologyType = TOPOLOGY_TYPE::NONE;
    std::vector<UINT>* mIndeices = nullptr;
    void* mVerteices = nullptr;
    std::vector<std::string>* mTextures = nullptr;
    MATERIAL_INFO* mMaterial = nullptr;
    std::string mStaticMaterial = "";
};

struct RS_SUBMESH_DATA
{
    D3D_PRIMITIVE_TOPOLOGY mTopologyType =
        D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    ID3D11InputLayout* mLayout = nullptr;
    ID3D11Buffer* mIndexBuffer = nullptr;
    ID3D11Buffer* mVertexBuffer = nullptr;
    UINT mIndexCount = 0;
    std::vector<std::string> mTextures = {};
    RS_MATERIAL_INFO mMaterial = {};
};

struct RS_SUBMESH_DRAWCALL_DATA
{
    D3D_PRIMITIVE_TOPOLOGY mTopologyType =
        D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    ID3D11InputLayout* mLayout = nullptr;
    ID3D11Buffer* mIndexBuffer = nullptr;
    ID3D11Buffer* mVertexBuffer = nullptr;
    UINT mIndexCount = 0;
};

struct RS_INSTANCE_DATA
{
    DirectX::XMFLOAT4X4 mWorldMat = {};
    RS_MATERIAL_INFO mMaterialData = {};
    DirectX::XMFLOAT4 mCustomizedData1 = {};
    DirectX::XMFLOAT4 mCustomizedData2 = {};
};

struct RS_INSTANCE_DRAWCALL_DATA
{
    std::vector<RS_INSTANCE_DATA>* mDataPtr = nullptr;
};

constexpr UINT MESH_TEX_MAX = 10;

struct RS_MESH_TEXTURE_INFO
{
    bool mUse = false;
    ID3D11ShaderResourceView* mSrv = nullptr;
};

struct RS_MISC_INFO
{
    DirectX::XMFLOAT2 mRtvSize;
    DirectX::XMFLOAT2 mInvRtvSize;
    float mDeltaTime;
};

struct RS_DRAWCALL_DATA
{
    RS_SUBMESH_DRAWCALL_DATA mMeshData = {};
    RS_INSTANCE_DRAWCALL_DATA mInstanceData = {};
    RS_MATERIAL_INFO mMaterialData = {};
    RS_MESH_TEXTURE_INFO mTextureDatas[MESH_TEX_MAX] = { {} };
    RS_MISC_INFO mMiscData = {};
};

struct RSDrawCallsPipe
{
    std::vector<RS_DRAWCALL_DATA> mDatas = {};
};

enum class RS_RESOURCE_TYPE
{
    BUFFER,
    TEXTURE1D,
    TEXTURE2D,
    TEXTURE3D,

    RESOURCE_SIZE
};

struct RS_RESOURCE_INFO
{
    RS_RESOURCE_TYPE mType = RS_RESOURCE_TYPE::BUFFER;

    union RS_RESOURCE_DATA
    {
        ID3D11Buffer* mBuffer = nullptr;
        ID3D11Texture1D* mTexture1D;
        ID3D11Texture2D* mTexture2D;
        ID3D11Texture3D* mTexture3D;
    } mResource;

    ID3D11RenderTargetView* mRtv = nullptr;
    ID3D11DepthStencilView* mDsv = nullptr;
    ID3D11ShaderResourceView* mSrv = nullptr;
    ID3D11UnorderedAccessView* mUav = nullptr;
};

#ifdef _RS_DX11
#include "RSExtraMacro.h"
#endif // _RS_DX11
#include "RSVertexType.h"
