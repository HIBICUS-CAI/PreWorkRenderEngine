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
    // TEMP----------------------
    return true;
    // TEMP----------------------
}

void RSDrawCallsPool::CleanAndStop()
{

}

void RSDrawCallsPool::AddDrawCallToPipe(
    DRAWCALL_TYPE _type, void* _data)
{

}

RSDrawCallsPipe* RSDrawCallsPool::GetDrawCallsPipe(
    DRAWCALL_TYPE _type)
{
    // TEMP----------------------
    return nullptr;
    // TEMP----------------------
}

void RSDrawCallsPool::ClearAllDrawCallsInPipes()
{

}
