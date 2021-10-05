//---------------------------------------------------------------
// File: RSLight.h
// Proj: RenderSystem_DX11
// Info: 对一个光源的基本内容进行描述及相关处理
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#include "RSLight.h"
#include "RSCamerasContainer.h"

RSLight::RSLight(LIGHT_INFO* _info) :
    mLightType(_info->mType),
    mWithShadow(_info->mWithShadow),
    mLightStrength(_info->mStrength),
    mLightDirection(_info->mDirection),
    mLightPosition(_info->mPosition),
    mLightFallOffStart(_info->mFalloffStart),
    mLightFallOffEnd(_info->mFalloffEnd),
    mLightSpotPower(_info->mSpotPower),
    mRSLightInfo({
        mLightStrength, mLightFallOffStart, mLightDirection,
        mLightFallOffEnd, mLightPosition, mLightSpotPower
        }),
    mRSLightCamera(nullptr)
{

}

RSLight::~RSLight()
{

}

RS_LIGHT_INFO* RSLight::GetRSLightInfo()
{
    return &mRSLightInfo;
}

void RSLight::ResetRSLight(LIGHT_INFO* _info)
{
    mLightType = _info->mType;
    SetRSLightStrength(_info->mStrength);
    SetRSLightDirection(_info->mDirection);
    SetRSLightPosition(_info->mPosition);
    SetRSLightFallOff(_info->mFalloffStart, _info->mFalloffEnd);
    SetRSLightSpotPower(_info->mSpotPower);
}

void RSLight::SetRSLightStrength(DirectX::XMFLOAT3 _strength)
{
    mLightStrength = _strength;
    mRSLightInfo.mStrength = _strength;
}

void RSLight::SetRSLightDirection(DirectX::XMFLOAT3 _direction)
{
    mLightDirection = _direction;
    mRSLightInfo.mDirection = _direction;
}

void RSLight::SetRSLightPosition(DirectX::XMFLOAT3 _position)
{
    mLightPosition = _position;
    mRSLightInfo.mPosition = _position;
}

void RSLight::SetRSLightFallOff(float _start, float _end)
{
    mLightFallOffStart = _start;
    mLightFallOffEnd = _end;
    mRSLightInfo.mFalloffStart = _start;
    mRSLightInfo.mFalloffEnd = _end;
}

void RSLight::SetRSLightSpotPower(float _power)
{
    mLightSpotPower = _power;
    mRSLightInfo.mSpotPower = _power;
}

RSCamera* RSLight::CreateLightCamera(std::string& _lightName,
    CAM_INFO* _info, RSCamerasContainer* _camContainer)
{
    if (!_info || !_camContainer) { return nullptr; }

    std::string name = _lightName + "-light-cam";
    mRSLightCamera = _camContainer->CreateRSCamera(name, _info);

    return mRSLightCamera;
}

RSCamera* RSLight::GetRSLightCamera()
{
    return mRSLightCamera;
}
