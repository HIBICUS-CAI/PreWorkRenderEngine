//---------------------------------------------------------------
// File: RSCommon.h
// Proj: RenderSystem_DX11
// Info: 声明RenderSystem_DX11相关的全局类型
// Date: 2021.9.14
// Mail: cai_genkan@outlook.com
// Comt: 在此lib项目内中提供ExtraMacro
//---------------------------------------------------------------

#pragma once

#include <vector>

enum class PASS_TYPE
{
    RENDER,
    COMPUTE
};

constexpr unsigned int RS_INVALID_ORDER = 0;

enum class LAYOUT_TYPE
{
    WITH_TANGENT,
    WITHOUT_TANGENT
};

enum class DRAWCALL_TYPE
{
    DEFAULT,
    MAX
};

struct RSDrawCallsPipe
{
    std::vector<double> mDouble = {};
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

struct DATA_TEXTURE_INFO
{
    int a = 0;
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

#ifdef _RS_DX11
#include "RSExtraMacro.h"
#endif // _RS_DX11
