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
    MIRROR_SELF,
    MIRRO_INSIDE,

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
    DirectX::XMFLOAT2 mOWidthAndheight = {};
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
    const std::vector<UINT>* const mIndeices = nullptr;
    const std::vector<void*>* const mVerteices = nullptr;
    const std::vector<std::string>* const mTextures = nullptr;
    const MATERIAL_INFO* const mMaterial = nullptr;
    std::string mStaticMaterial = "";
};

struct RS_SUBMESH_DATA
{
    D3D_PRIMITIVE_TOPOLOGY mTopologyType =
        D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    ID3D11InputLayout* mLayout = nullptr;
    ID3D11Buffer* mIndexBuffer = nullptr;
    ID3D11Buffer* mVertexBuffer = nullptr;
    std::vector<std::string> mTextures = {};
    RS_MATERIAL_INFO mMaterial = {};
};

constexpr UINT MESH_TEX_MAX = 10;

struct RS_MESH_TEXTURE_INFO
{
    bool mUse = false;
    int a = 0;
};

struct RS_MISC_INFO
{
    DirectX::XMFLOAT2 mRtvSize;
    DirectX::XMFLOAT2 mInvRtvSize;
    float mDeltaTime;
};

// 不包含光照信息，之后从相关地方调取
struct RS_DRAWCALL_DATA
{
    RS_SUBMESH_DATA mMeshData = {};
    RS_CAM_INFO mCameraData = {};
    RS_MATERIAL_INFO mMaterialData = {};
    RS_MESH_TEXTURE_INFO mTextureDatas[MESH_TEX_MAX] = { {} };
    RS_MISC_INFO mMiscData = {};
};

struct RSDrawCallsPipe
{
    std::vector<RS_DRAWCALL_DATA> mDatas = {};
};

struct DATA_TEXTURE_INFO
{
    ID3D11Texture2D* mTexture = nullptr;
    ID3D11RenderTargetView* mRtv = nullptr;
    ID3D11DepthStencilView* mDsv = nullptr;
    ID3D11ShaderResourceView* mSrv = nullptr;
    ID3D11UnorderedAccessView* mUav = nullptr;
};

#ifdef _RS_DX11
#include "RSExtraMacro.h"
#endif // _RS_DX11
