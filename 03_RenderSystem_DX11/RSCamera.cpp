//---------------------------------------------------------------
// File: RSCamera.cpp
// Proj: RenderSystem_DX11
// Info: 对一个摄像头的基本内容进行描述及相关处理
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#include "RSCamera.h"

RSCamera::RSCamera(CAM_INFO* _info) :
    mLensType(LENS_TYPE::ORTHOGRAPHIC),
    mCamPosition({ 0.f,0.f,0.f }), mCamUpVec({ 0.f,0.f,0.f }),
    mCamLookAt({ 0.f,0.f,0.f }), mFovAngleYPersp(0.f),
    mAspectRatioPersp(0.f), mWidthOrtho(0.f), mHeightOrtho(0.f),
    mNearZ(0.f), mFarZ(0.f), mRSCameraInfo({}),
    mProjMatrix({
        0.f,0.f,0.f,0.f,
        0.f,0.f,0.f,0.f,
        0.f,0.f,0.f,0.f,
        0.f,0.f,0.f,0.f
        }),
    mInvProjMatrix({
        0.f,0.f,0.f,0.f,
        0.f,0.f,0.f,0.f,
        0.f,0.f,0.f,0.f,
        0.f,0.f,0.f,0.f
        }),
    mViewMatrix({
        0.f,0.f,0.f,0.f,
        0.f,0.f,0.f,0.f,
        0.f,0.f,0.f,0.f,
        0.f,0.f,0.f,0.f
        })
{
    // TEMP---------------------
    _info->a = 0;
    // TEMP---------------------
}

RSCamera::~RSCamera()
{

}

RS_CAM_INFO* RSCamera::GetRSCameraInfo()
{
    return &mRSCameraInfo;
}

void RSCamera::ResetRSCamera(CAM_INFO* _info)
{
    // TEMP---------------------
    _info->a = 0;
    // TEMP---------------------
}

void RSCamera::TranslateRSCamera(DirectX::XMFLOAT3 _delta)
{
    mCamPosition.x += _delta.x;
    mCamPosition.y += _delta.y;
    mCamPosition.z += _delta.z;
    CalcRSCameraInfo();
}

void RSCamera::RotateRSCamera(DirectX::XMFLOAT3 _deltaAngle)
{
    // TEMP---------------------
    mCamLookAt.x += _deltaAngle.x;
    mCamLookAt.y += _deltaAngle.y;
    mCamLookAt.z += _deltaAngle.z;
    // TEMP---------------------
    CalcRSCameraInfo();
}

void RSCamera::ChangeRSCameraFovY(float _angle)
{
    // TEMP---------------------
    mFovAngleYPersp = _angle;
    // TEMP---------------------
    CalcRSCameraInfo();
}

void RSCamera::ChangeRSCameraNearFarZ(float _near, float _far)
{
    mNearZ = _near;
    mFarZ = _far;
    CalcRSCameraInfo();
}

void RSCamera::CalcRSCameraInfo()
{
    // TEMP---------------------
    mRSCameraInfo.a = 0;
    // TEMP---------------------
}
