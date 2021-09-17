//---------------------------------------------------------------
// File: RSExtraMacro.h
// Proj: RenderSystem_DX11
// Info: 提供RenderSystem_DX11内可用的额外宏定义
// Date: 2021.9.14
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#pragma once

#include <Windows.h>

#define FAIL_HR_RETURN(r) { if (FAILED((r))) { return false; } }
#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p)=nullptr; } }
