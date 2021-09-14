//---------------------------------------------------------------
// File: RSRoot_DX11.cpp
// Proj: RenderSystem_DX11
// Info: 保存并提供此RenderSystem相关的重要内容引用
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxguid.lib")

#include "RSRoot_DX11.h"
#include "RSDevices.h"
#include "RSPipelinesManager.h"
#include "RSDrawCallsPool.h"

RSRoot_DX11::RSRoot_DX11() :
    mDevicesPtr(nullptr), mPipelinesManagerPtr(nullptr),
    mDrawCallsPoolPtr(nullptr), mTexturesManagerPtr(nullptr),
    mStaticResourcesPtr(nullptr), mCamerasContainerPtr(nullptr),
    mLightsContainerPtr(nullptr), mMeshHelperPtr(nullptr)
{

}

RSRoot_DX11::~RSRoot_DX11()
{

}

bool RSRoot_DX11::StartUp(HWND _wndHandle)
{
    mDevicesPtr = new RSDevices();
    if (!mDevicesPtr->StartUp(this, _wndHandle))
    {
        return false;
    }

    mPipelinesManagerPtr = new RSPipelinesManager();
    if (!mPipelinesManagerPtr->StartUp(this))
    {
        return false;
    }

    mDrawCallsPoolPtr = new RSDrawCallsPool();
    if (!mDrawCallsPoolPtr->StartUp(this))
    {
        return false;
    }

    return true;
}

void RSRoot_DX11::CleanAndStop()
{
    if (mDrawCallsPoolPtr)
    {
        mDrawCallsPoolPtr->CleanAndStop();
        delete mDrawCallsPoolPtr;
        mDrawCallsPoolPtr = nullptr;
    }

    if (mPipelinesManagerPtr)
    {
        mPipelinesManagerPtr->CleanAndStop();
        delete mPipelinesManagerPtr;
        mPipelinesManagerPtr = nullptr;
    }

    if (mDevicesPtr)
    {
        mDevicesPtr->CleanAndStop();
        delete mDevicesPtr;
        mDevicesPtr = nullptr;
    }
}

RSDevices* RSRoot_DX11::Devices() const
{
    return mDevicesPtr;
}

RSPipelinesManager* RSRoot_DX11::PipelinesManager() const
{
    return mPipelinesManagerPtr;
}

RSDrawCallsPool* RSRoot_DX11::DrawCallsPool() const
{
    return mDrawCallsPoolPtr;
}

RSTexturesManager* RSRoot_DX11::TexturesManager() const
{
    return mTexturesManagerPtr;
}

RSStaticResources* RSRoot_DX11::StaticResources() const
{
    return mStaticResourcesPtr;
}

RSCamerasContainer* RSRoot_DX11::CamerasContainer() const
{
    return mCamerasContainerPtr;
}

RSLightsContainer* RSRoot_DX11::LightsContainer() const
{
    return mLightsContainerPtr;
}

RSMeshHelper* RSRoot_DX11::MeshHelper() const
{
    return mMeshHelperPtr;
}
