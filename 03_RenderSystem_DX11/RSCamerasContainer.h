//---------------------------------------------------------------
// File: RSCamerasContainer.h
// Proj: RenderSystem_DX11
// Info: �����е�������������ü��򵥴���
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#pragma once

#include <string>
#include <unordered_map>
#include "RSCamera.h"

class RSCamerasContainer
{
public:
    RSCamerasContainer();
    ~RSCamerasContainer();

    bool StartUp(class RSRoot_DX11* _root);
    void CleanAndStop();

    RSCamera* CreateRSCamera(std::string& _name, CAM_INFO* _info);
    RSCamera* GetRSCamera(std::string& _name);
    RS_CAM_INFO* GetRSCameraInfo(std::string& _name);
    void DeleteRSCamera(std::string& _name);

private:
    class RSRoot_DX11* mRootPtr;

    std::unordered_map<std::string, RSCamera> mCameraMap;
};

