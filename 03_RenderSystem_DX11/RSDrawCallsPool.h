//---------------------------------------------------------------
// File: RSDrawCallsPool.h
// Proj: RenderSystem_DX11
// Info: 管理所有保存着绘制图元的通道
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#pragma once

#include "RSCommon.h"
#include <array>

class RSDrawCallsPool
{
public:
    RSDrawCallsPool();
    ~RSDrawCallsPool();

    bool StartUp(class RSRoot_DX11* _root);
    void CleanAndStop();

    void AddDrawCallToPipe(DRAWCALL_TYPE _type, void* _data);
    RSDrawCallsPipe* GetDrawCallsPipe(DRAWCALL_TYPE _type);

    void ClearAllDrawCallsInPipes();

private:
    class RSRoot_DX11* mRootPtr;
    std::array<RSDrawCallsPipe, (size_t)DRAWCALL_TYPE::MAX>
        mDrawCallsArray;
};

