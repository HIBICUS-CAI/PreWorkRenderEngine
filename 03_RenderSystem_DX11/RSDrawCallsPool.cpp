//---------------------------------------------------------------
// File: RSDrawCallsPool.cpp
// Proj: RenderSystem_DX11
// Info: 管理所有保存着绘制图元的通道
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#include "RSDrawCallsPool.h"
#include "RSRoot_DX11.h"

RSDrawCallsPool::RSDrawCallsPool() :
    mRootPtr(nullptr), mDrawCallsArray({ {} })
{

}

RSDrawCallsPool::~RSDrawCallsPool()
{

}

bool RSDrawCallsPool::StartUp(RSRoot_DX11* _root)
{
    if (!_root) { return false; }

    mRootPtr = _root;

    return true;
}

void RSDrawCallsPool::CleanAndStop()
{
    // TEMP----------------------
    for (auto& pipe : mDrawCallsArray)
    {
        pipe.mDouble.clear();
    }
    // TEMP----------------------
}

void RSDrawCallsPool::AddDrawCallToPipe(
    DRAWCALL_TYPE _type, void* _data)
{
    // TEMP----------------------
    mDrawCallsArray[(size_t)_type].mDouble.push_back(
        *(double*)_data);
    // TEMP----------------------
}

RSDrawCallsPipe* RSDrawCallsPool::GetDrawCallsPipe(
    DRAWCALL_TYPE _type)
{
    return &mDrawCallsArray[(size_t)_type];
}

void RSDrawCallsPool::ClearAllDrawCallsInPipes()
{
    // TEMP----------------------
    for (auto& pipe : mDrawCallsArray)
    {
        pipe.mDouble.clear();
    }
    // TEMP----------------------
}
