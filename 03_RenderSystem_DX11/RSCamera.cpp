//---------------------------------------------------------------
// File: RSCamera.cpp
// Proj: RenderSystem_DX11
// Info: 对一个摄像头的基本内容进行描述及相关处理
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#include "RSCamera.h"

using namespace DirectX;

RSCamera::RSCamera(CAM_INFO* _info) :
    mLensType(LENS_TYPE::ORTHOGRAPHIC),
    mCamPosition({ 0.f,0.f,0.f }), mCamUpVec({ 0.f,0.f,0.f }),
    mCamLookAt({ 0.f,0.f,0.f }), mFovAngleYPersp(0.f),
    mAspectRatio(0.f), mWidthOrtho(0.f), mHeightOrtho(0.f),
    mNearZ(0.f), mFarZ(0.f), mRSCameraInfo({})
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

void RSCamera::TranslateRSCamera(XMFLOAT3 _delta)
{
    XMVECTOR pos = XMLoadFloat3(&mCamPosition);
    XMVECTOR delta = XMLoadFloat3(&_delta);
    pos += delta;
    XMStoreFloat3(&mCamPosition, pos);

    CalcRSViewMat();
}

void RSCamera::RotateRSCamera(XMFLOAT3 _deltaAngle)
{
    XMVECTOR lookat = XMLoadFloat3(&mCamLookAt);
    XMVECTOR up = XMLoadFloat3(&mCamUpVec);
    XMVECTOR pos = XMLoadFloat3(&mCamPosition);
    lookat -= pos;
    lookat = XMVector3Rotate(lookat,
        XMQuaternionRotationRollPitchYaw(
            _deltaAngle.x, _deltaAngle.y, _deltaAngle.z));
    lookat += pos;
    up = XMVector3Rotate(up,
        XMQuaternionRotationRollPitchYaw(
            _deltaAngle.x, _deltaAngle.y, _deltaAngle.z));
    XMStoreFloat3(&mCamLookAt, lookat);
    XMStoreFloat3(&mCamUpVec, up);

    CalcRSViewMat();
}

void RSCamera::ChangeRSCameraFovY(float _p_angle_o_height)
{
    switch (mLensType)
    {
    case LENS_TYPE::PERSPECTIVE:
        mFovAngleYPersp = _p_angle_o_height;
        break;
    case LENS_TYPE::ORTHOGRAPHIC:
        mHeightOrtho = _p_angle_o_height;
        mWidthOrtho = mAspectRatio * mHeightOrtho;
        break;
    default:
        return;
    }

    CalcRSProjMat();
}

void RSCamera::ChangeRSCameraNearFarZ(float _near, float _far)
{
    mNearZ = _near;
    mFarZ = _far;

    CalcRSProjMat();
}

void RSCamera::CalcRSViewMat()
{
    XMMATRIX view = XMMatrixLookAtLH(
        XMLoadFloat3(&mCamPosition),
        XMLoadFloat3(&mCamLookAt),
        XMLoadFloat3(&mCamUpVec));
    XMVECTOR det = XMMatrixDeterminant(view);
    XMMATRIX invview = XMMatrixInverse(&det, view);
    XMMATRIX viewproj = XMMatrixMultiply(view,
        XMLoadFloat4x4(&mRSCameraInfo.mProjMat));
    XMStoreFloat4x4(&mRSCameraInfo.mViewMat, view);
    XMStoreFloat4x4(&mRSCameraInfo.mInvViewMat, invview);
    XMStoreFloat4x4(&mRSCameraInfo.mViewProjMat, viewproj);
    mRSCameraInfo.mEyePosition = mCamPosition;
}

void RSCamera::CalcRSProjMat()
{
    XMMATRIX proj = {};
    XMVECTOR det = {};
    XMMATRIX invproj = {};
    XMMATRIX viewproj = {};
    switch (mLensType)
    {
    case LENS_TYPE::PERSPECTIVE:
        proj = XMMatrixPerspectiveFovLH(
            mFovAngleYPersp, mAspectRatio, mNearZ, mFarZ);
        det = XMMatrixDeterminant(proj);
        invproj = XMMatrixInverse(&det, proj);
        viewproj = XMMatrixMultiply(
            XMLoadFloat4x4(&mRSCameraInfo.mViewMat), proj);
        XMStoreFloat4x4(&mRSCameraInfo.mProjMat, proj);
        XMStoreFloat4x4(&mRSCameraInfo.mInvProjMat, invproj);
        XMStoreFloat4x4(&mRSCameraInfo.mViewProjMat, viewproj);
        return;
    case LENS_TYPE::ORTHOGRAPHIC:
        proj = XMMatrixOrthographicLH(
            mWidthOrtho, mHeightOrtho, mNearZ, mFarZ);
        det = XMMatrixDeterminant(proj);
        invproj = XMMatrixInverse(&det, proj);
        viewproj = XMMatrixMultiply(
            XMLoadFloat4x4(&mRSCameraInfo.mViewMat), proj);
        XMStoreFloat4x4(&mRSCameraInfo.mProjMat, proj);
        XMStoreFloat4x4(&mRSCameraInfo.mInvProjMat, invproj);
        XMStoreFloat4x4(&mRSCameraInfo.mViewProjMat, viewproj);
        return;
    default:
        return;
    }
}
