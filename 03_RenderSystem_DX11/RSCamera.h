//---------------------------------------------------------------
// File: RSCamera.h
// Proj: RenderSystem_DX11
// Info: 对一个摄像头的基本内容进行描述及相关处理
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#pragma once

#include "RSCommon.h"

class RSCamera
{
public:
    RSCamera(CAM_INFO* _info);
    ~RSCamera();

    RS_CAM_INFO* GetRSCameraInfo();
    void ResetRSCamera(CAM_INFO* _info);

    void TranslateRSCamera(DirectX::XMFLOAT3 _delta);
    void RotateRSCamera(DirectX::XMFLOAT3 _deltaAngle);
    void ChangeRSCameraFovY(float _angle);
    void ChangeRSCameraNearFarZ(float _near, float _far);

private:
    void CalcRSCameraInfo();

private:
    LENS_TYPE mLensType;

    DirectX::XMFLOAT3 mCamPosition;
    DirectX::XMFLOAT3 mCamUpVec;
    DirectX::XMFLOAT3 mCamLookAt;

    float mFovAngleYPersp;
    float mAspectRatioPersp;
    float mWidthOrtho;
    float mHeightOrtho;
    float mNearZ;
    float mFarZ;

    DirectX::XMFLOAT4X4 mProjMatrix;
    DirectX::XMFLOAT4X4 mInvProjMatrix;
    DirectX::XMFLOAT4X4 mViewMatrix;

    RS_CAM_INFO mRSCameraInfo;
};

