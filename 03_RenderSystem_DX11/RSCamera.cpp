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

}

RSCamera::~RSCamera()
{

}

RS_CAM_INFO* RSCamera::GetRSCameraInfo() const
{
    // TEMP----------------------
    return nullptr;
    // TEMP----------------------
}

void RSCamera::ResetRSCamera(CAM_INFO* _info)
{

}

void RSCamera::TranslateRSCamera(DirectX::XMFLOAT3 _delta)
{

}

void RSCamera::RotateRSCamera(DirectX::XMFLOAT3 _deltaAngle)
{

}

void RSCamera::ChangeRSCameraFovY(float _angle)
{

}

void RSCamera::ChangeRSCameraNearFarZ(float _near, float _far)
{

}

void RSCamera::CalcRSCameraInfo()
{

}
