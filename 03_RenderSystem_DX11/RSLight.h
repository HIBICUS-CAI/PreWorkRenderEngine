//---------------------------------------------------------------
// File: RSLight.h
// Proj: RenderSystem_DX11
// Info: 对一个光源的基本内容进行描述及相关处理
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#pragma once

#include "RSCommon.h"
#include <DirectXMath.h>

class RSLight
{
public:
    RSLight(LIGHT_INFO* _info);
    ~RSLight();

    RS_LIGHT_INFO* GetRSLightInfo();
    void ResetRSLight(LIGHT_INFO* _info);

    void SetRSLightStrength(DirectX::XMFLOAT3 _strength);
    void SetRSLightDirection(DirectX::XMFLOAT3 _direction);
    void SetRSLightPosition(DirectX::XMFLOAT3 _position);
    void SetRSLightFallOff(float _start, float _end);
    void SetRSLightSpotPower(float _power);

private:
    LIGHT_TYPE mLightType;

    DirectX::XMFLOAT3 mLightStrength;
    DirectX::XMFLOAT3 mLightDirection;
    DirectX::XMFLOAT3 mLightPosition;
    float mLightFallOffStart;
    float mLightFallOffEnd;
    float mLightSpotPower;

    RS_LIGHT_INFO mRSLightInfo;
};

