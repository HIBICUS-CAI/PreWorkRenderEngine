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
#include "RSTexturesManager.h"
#include "RSStaticResources.h"
#include "RSLightsContainer.h"
#include "RSCamerasContainer.h"
#include "RSParticalsContainer.h"
#include "RSMeshHelper.h"
#include <assert.h>

static RSRoot_DX11* g_RSRoot_DX11_Singleton = nullptr;

RSRoot_DX11* GetRSRoot_DX11_Singleton()
{
    if (!g_RSRoot_DX11_Singleton)
    {
        bool rs_root_dx11_hasnt_been_built = false;
        assert(rs_root_dx11_hasnt_been_built);
    }
    return g_RSRoot_DX11_Singleton;
}

RSRoot_DX11::RSRoot_DX11() :
    mDevicesPtr(nullptr), mPipelinesManagerPtr(nullptr),
    mDrawCallsPoolPtr(nullptr), mTexturesManagerPtr(nullptr),
    mStaticResourcesPtr(nullptr), mCamerasContainerPtr(nullptr),
    mLightsContainerPtr(nullptr), mMeshHelperPtr(nullptr),
    mParticalsContainerPtr(nullptr)
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

    mTexturesManagerPtr = new RSTexturesManager();
    if (!mTexturesManagerPtr->StartUp(this))
    {
        return false;
    }

    mStaticResourcesPtr = new RSStaticResources();
    if (!mStaticResourcesPtr->StartUp(this))
    {
        return false;
    }

    mLightsContainerPtr = new RSLightsContainer();
    if (!mLightsContainerPtr->StartUp(this))
    {
        return false;
    }

    mCamerasContainerPtr = new RSCamerasContainer();
    if (!mCamerasContainerPtr->StartUp(this))
    {
        return false;
    }

    mParticalsContainerPtr = new RSParticalsContainer();
    if (!mParticalsContainerPtr->StartUp(this))
    {
        return false;
    }

    mMeshHelperPtr = new RSMeshHelper();
    if (!mMeshHelperPtr->StartUp(this, mTexturesManagerPtr))
    {
        return false;
    }

    if (g_RSRoot_DX11_Singleton)
    {
        bool rs_root_dx11_should_be_singleton = false;
        assert(rs_root_dx11_should_be_singleton);
        return false;
    }
    g_RSRoot_DX11_Singleton = this;

    return true;
}

void RSRoot_DX11::CleanAndStop()
{
    if (mPipelinesManagerPtr)
    {
        mPipelinesManagerPtr->CleanAndStop();
        delete mPipelinesManagerPtr;
        mPipelinesManagerPtr = nullptr;
    }

    if (mMeshHelperPtr)
    {
        mMeshHelperPtr->CleanAndStop();
        delete mMeshHelperPtr;
        mMeshHelperPtr = nullptr;
    }

    if (mCamerasContainerPtr)
    {
        mCamerasContainerPtr->CleanAndStop();
        delete mCamerasContainerPtr;
        mCamerasContainerPtr = nullptr;
    }

    if (mLightsContainerPtr)
    {
        mLightsContainerPtr->CleanAndStop();
        delete mLightsContainerPtr;
        mLightsContainerPtr = nullptr;
    }

    if (mParticalsContainerPtr)
    {
        mParticalsContainerPtr->CleanAndStop();
        delete mParticalsContainerPtr;
        mParticalsContainerPtr = nullptr;
    }

    if (mStaticResourcesPtr)
    {
        mStaticResourcesPtr->CleanAndStop();
        delete mStaticResourcesPtr;
        mStaticResourcesPtr = nullptr;
    }

    if (mTexturesManagerPtr)
    {
        mTexturesManagerPtr->CleanAndStop();
        delete mTexturesManagerPtr;
        mTexturesManagerPtr = nullptr;
    }

    if (mDrawCallsPoolPtr)
    {
        mDrawCallsPoolPtr->CleanAndStop();
        delete mDrawCallsPoolPtr;
        mDrawCallsPoolPtr = nullptr;
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

RSParticalsContainer* RSRoot_DX11::ParticalsContainer() const
{
    return mParticalsContainerPtr;
}

RSMeshHelper* RSRoot_DX11::MeshHelper() const
{
    return mMeshHelperPtr;
}
