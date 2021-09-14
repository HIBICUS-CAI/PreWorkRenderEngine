//---------------------------------------------------------------
// File: RSExtraMacro.h
// Proj: RenderSystem_DX11
// Info: �ṩRenderSystem_DX11�ڿ��õĶ���궨��
// Date: 2021.9.14
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#pragma once

#include <Windows.h>

#define FAIL_HR_RETURN(r) { if (FAILED((r))) { return false; } }
#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p)=nullptr; } }
