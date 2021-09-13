//---------------------------------------------------------------
// File: RSLight.h
// Proj: RenderSystem_DX11
// Info: 对一个光源的基本内容进行描述及相关处理
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#include "RSLight.h"

RSLight::RSLight(LIGHT_INFO* _info) :
    mLightType(LIGHT_TYPE::DIRECT), mLightStrength({ 0.f,0.f,0.f }),
    mLightDirection({ 0.f,0.f,0.f }), mLightPosition({ 0.f,0.f,0.f }),
    mLightFallOffStart(0.f), mLightFallOffEnd(0.f),
    mLightSpotPower(0.f), mRSLightInfo({})
{

}

RSLight::~RSLight()
{

}

RS_LIGHT_INFO* RSLight::GetRSLightInfo()
{
    // TEMP----------------------
    return nullptr;
    // TEMP----------------------
}

void RSLight::ResetRSLight(LIGHT_INFO* _info)
{

}

void RSLight::SetRSLightStrength(DirectX::XMFLOAT3 _strength)
{

}

void RSLight::SetRSLightDirection(DirectX::XMFLOAT3 _direction)
{

}

void RSLight::SetRSLightPosition(DirectX::XMFLOAT3 _position)
{

}

void RSLight::SetRSLightFallOff(float _start, float _end)
{

}

void RSLight::SetRSLightSpotPower(float _power)
{

}
