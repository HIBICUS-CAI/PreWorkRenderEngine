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
#include "RSCamera.h"

RSCamerasContainer::RSCamerasContainer() :
    mRootPtr(nullptr), mCameraMap({})
{

}

RSCamerasContainer::~RSCamerasContainer()
{

}

bool RSCamerasContainer::StartUp(RSRoot_DX11* _root)
{
    if (!_root) { return false; }

    mRootPtr = _root;

    return true;
}

void RSCamerasContainer::CleanAndStop()
{
    for (auto& cam : mCameraMap)
    {
        delete cam.second;
    }
    mCameraMap.clear();
}

RSCamera* RSCamerasContainer::CreateRSCamera(
    std::string& _name, CAM_INFO* _info)
{
    if (!_info) { return nullptr; }

    if (mCameraMap.find(_name) == mCameraMap.end())
    {
        RSCamera* cam = new RSCamera(_info);
        mCameraMap.insert({ _name,cam });
    }

    return mCameraMap[_name];
}

RSCamera* RSCamerasContainer::GetRSCamera(std::string& _name)
{
    auto found = mCameraMap.find(_name);
    if (found != mCameraMap.end())
    {
        return found->second;
    }
    else
    {
        return nullptr;
    }
}

RS_CAM_INFO* RSCamerasContainer::GetRSCameraInfo(
    std::string& _name)
{
    auto found = mCameraMap.find(_name);
    if (found != mCameraMap.end())
    {
        return found->second->GetRSCameraInfo();
    }
    else
    {
        return nullptr;
    }
}

void RSCamerasContainer::DeleteRSCamera(std::string& _name)
{
    auto found = mCameraMap.find(_name);
    if (found != mCameraMap.end())
    {
        delete found->second;
        mCameraMap.erase(found);
    }
}
