//---------------------------------------------------------------
// File: RSCamerasContainer.cpp
// Proj: RenderSystem_DX11
// Info: 对所有的摄像机进行引用及简单处理
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#include "RSCamerasContainer.h"
#include "RSRoot_DX11.h"

RSCamerasContainer::RSCamerasContainer() :
    mRootPtr(nullptr), mCameraMap({})
{

}

RSCamerasContainer::~RSCamerasContainer()
{

}

bool RSCamerasContainer::StartUp(RSRoot_DX11* _root)
{
    // TEMP----------------------
    return true;
    // TEMP----------------------
}

void RSCamerasContainer::CleanAndStop()
{

}

RSCamera* RSCamerasContainer::CreateRSCamera(
    std::string& _name, CAM_INFO* _info)
{
    // TEMP----------------------
    return nullptr;
    // TEMP----------------------
}

RSCamera* RSCamerasContainer::GetRSCamera(std::string& _name)
{
    // TEMP----------------------
    return nullptr;
    // TEMP----------------------
}

RS_CAM_INFO* RSCamerasContainer::GetRSCameraInfo(
    std::string& _name)
{
    // TEMP----------------------
    return nullptr;
    // TEMP----------------------
}

void RSCamerasContainer::DeleteRSCamera(std::string& _name)
{

}
