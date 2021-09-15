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
    double a = 0.0;
};

struct RS_CAM_INFO
{
    double a = 0.0;
};

enum class LIGHT_TYPE
{
    DIRECT,
    POINT,
    SPOT
};

struct LIGHT_INFO
{
    double a = 0.0;
};

struct RS_LIGHT_INFO
{
    double a = 0.0;
};

struct MATERIAL_INFO
{
    int a = 0;
};

struct RS_MATERIAL_INFO
{
    double a = 0.0;
};

struct SUBMESH_INFO
{
    int a = 0;
};

struct RS_SUBMESH_DATA
{
    int a = 0;
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
    int a = 0;
};

#ifdef _RS_DX11
#include "RSExtraMacro.h"
#endif // _RS_DX11
