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
    for (auto& pipe : mDrawCallsArray)
    {
        pipe.mDatas.clear();
    }
}

void RSDrawCallsPool::AddDrawCallToPipe(
    DRAWCALL_TYPE _type, RS_DRAWCALL_DATA& _data)
{
    mDrawCallsArray[(size_t)_type].mDatas.emplace_back(_data);
}

RSDrawCallsPipe* RSDrawCallsPool::GetDrawCallsPipe(
    DRAWCALL_TYPE _type)
{
    return &mDrawCallsArray[(size_t)_type];
}

void RSDrawCallsPool::ClearAllDrawCallsInPipes()
{
    for (auto& pipe : mDrawCallsArray)
    {
        pipe.mDatas.clear();
    }
}
