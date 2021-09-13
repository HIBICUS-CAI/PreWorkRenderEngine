//---------------------------------------------------------------
// File: RSRoot_DX11.cpp
// Proj: RenderSystem_DX11
// Info: 保存并提供此RenderSystem相关的重要内容引用
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#include "RSRoot_DX11.h"

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
    // TEMP----------------------
    return true;
    // TEMP----------------------
}

void RSRoot_DX11::CleanAndStop()
{

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
