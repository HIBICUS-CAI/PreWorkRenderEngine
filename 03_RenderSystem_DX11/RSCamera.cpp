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
    mLensType = _info->mType;
    mCamPosition = _info->mPosition;
    mCamUpVec = _info->mUpVec;
    mCamLookAt = _info->mLookAt;
    mFovAngleYPersp = _info->mPFovyAndRatio.x;
    mWidthOrtho = _info->mOWidthAndHeight.x;
    mHeightOrtho = _info->mOWidthAndHeight.y;
    mNearZ = _info->mNearFarZ.x;
    mFarZ = _info->mNearFarZ.y;
    switch (mLensType)
    {
    case LENS_TYPE::PERSPECTIVE:
        mAspectRatio = _info->mPFovyAndRatio.y;
        break;
    case LENS_TYPE::ORTHOGRAPHIC:
        mAspectRatio = mWidthOrtho / mHeightOrtho;
        break;
    default:
        break;
    }

    CalcRSViewMat();
    CalcRSProjMat();
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
    mLensType = _info->mType;
    mCamPosition = _info->mPosition;
    mCamUpVec = _info->mUpVec;
    mCamLookAt = _info->mLookAt;
    mFovAngleYPersp = _info->mPFovyAndRatio.x;
    mWidthOrtho = _info->mOWidthAndHeight.x;
    mHeightOrtho = _info->mOWidthAndHeight.y;
    mNearZ = _info->mNearFarZ.x;
    mFarZ = _info->mNearFarZ.y;
    switch (mLensType)
    {
    case LENS_TYPE::PERSPECTIVE:
        mAspectRatio = _info->mPFovyAndRatio.y;
        break;
    case LENS_TYPE::ORTHOGRAPHIC:
        mAspectRatio = mWidthOrtho / mHeightOrtho;
        break;
    default:
        break;
    }

    CalcRSViewMat();
    CalcRSProjMat();
}

void RSCamera::TranslateRSCamera(XMFLOAT3 _delta)
{
    XMVECTOR lookat = XMLoadFloat3(&mCamLookAt);
    XMVECTOR up = XMLoadFloat3(&mCamUpVec);
    XMVECTOR right = XMVector3Cross(lookat, up);
    XMVECTOR movelookat = XMVector3Normalize(lookat);
    XMVECTOR moveright = XMVector3Normalize(right);
    movelookat *= _delta.z;
    moveright *= _delta.x;
    XMVECTOR pos = XMLoadFloat3(&mCamPosition);
    pos += movelookat;
    pos += moveright;
    XMStoreFloat3(&mCamPosition, pos);

    CalcRSViewMat();
}

void RSCamera::RotateRSCamera(float _vertical, float _horizontal)
{
    XMVECTOR lookat = XMLoadFloat3(&mCamLookAt);
    XMVECTOR up = XMLoadFloat3(&mCamUpVec);
    XMVECTOR right = XMVector3Cross(lookat, up);
    XMMATRIX pitch = XMMatrixRotationAxis(right, _vertical);
    up = XMVector3TransformNormal(up, pitch);
    lookat = XMVector3TransformNormal(lookat, pitch);
    XMMATRIX y = XMMatrixRotationY(_horizontal);
    up = XMVector3TransformNormal(up, y);
    lookat = XMVector3TransformNormal(lookat, y);
    XMStoreFloat3(&mCamLookAt, lookat);
    XMStoreFloat3(&mCamUpVec, up);

    CalcRSViewMat();
}

void RSCamera::RotateRSCamera(XMFLOAT3 _deltaAngle)
{
    XMVECTOR lookat = XMLoadFloat3(&mCamLookAt);
    XMVECTOR up = XMLoadFloat3(&mCamUpVec);
    XMMATRIX x = XMMatrixRotationX(_deltaAngle.x);
    up = XMVector3TransformNormal(up, x);
    lookat = XMVector3TransformNormal(lookat, x);
    XMMATRIX y = XMMatrixRotationY(_deltaAngle.y);
    up = XMVector3TransformNormal(up, y);
    lookat = XMVector3TransformNormal(lookat, y);
    XMMATRIX z = XMMatrixRotationZ(_deltaAngle.z);
    up = XMVector3TransformNormal(up, z);
    lookat = XMVector3TransformNormal(lookat, z);
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

DirectX::XMFLOAT3 RSCamera::GetRSCameraPosition()
{
    return mCamPosition;
}

void RSCamera::ChangeRSCameraPosition(DirectX::XMFLOAT3& _position)
{
    mCamPosition = _position;

    CalcRSViewMat();
}

void RSCamera::ChangeRSCameraPosition(DirectX::XMFLOAT3&& _position)
{
    mCamPosition = _position;

    CalcRSViewMat();
}

void RSCamera::ResetRSCameraRotation(DirectX::XMFLOAT3 _lookAt,
    DirectX::XMFLOAT3 _upVec)
{
    mCamLookAt = _lookAt;
    mCamUpVec = _upVec;

    CalcRSViewMat();
}

void RSCamera::CalcRSViewMat()
{
    XMMATRIX view = XMMatrixLookAtLH(
        XMLoadFloat3(&mCamPosition),
        XMLoadFloat3(&mCamLookAt) + XMLoadFloat3(&mCamPosition),
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
