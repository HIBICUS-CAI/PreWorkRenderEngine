#include "light_helper.hlsli"

struct VS_OUTPUT
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION0;
    float4 ShadowPosH : POSITION1;
    float4 SsaoPosH : POSITION2;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 TexCoordL : TEXCOORD;
    uint UseBumped : BLENDINDICES;
};

struct AMBIENT
{
    float4 gAmbient;
};

struct LIGHT_INFO
{
    float3 gCameraPos;
    float gPad0;
    uint gDirectLightNum;
    uint gSpotLightNum;
    uint gPointLightNum;
    uint gPad1;
};

SamplerState gSampler : register(s0);
SamplerComparisonState gShadowComSam : register(s1);

StructuredBuffer<AMBIENT> gAmbient : register(t0);
StructuredBuffer<LIGHT_INFO> gLightInfo : register(t1);
StructuredBuffer<MATERIAL> gMaterial : register(t2);
StructuredBuffer<LIGHT> gLights : register(t3);

Texture2D gDiffuse : register(t4);
Texture2D<float> gShadowMap : register(t5);
Texture2D gSsaoMap : register(t6);
Texture2D gBumpedMap : register(t7);

float CalcShadowFactor(float4 _shadowPosH)
{
    _shadowPosH.xyz /= _shadowPosH.w;
    _shadowPosH.x = 0.5f * _shadowPosH.x + 0.5f;
    _shadowPosH.y = -0.5f * _shadowPosH.y + 0.5f;

    uint width, height, numMips;
    gShadowMap.GetDimensions(0, width, height, numMips);

    const float WIDTH = (float)width;
    const float DX = 1.0f / WIDTH;
    const float HEIGHT = (float)height;
    const float DY = 1.0f / HEIGHT;

    float depth = _shadowPosH.z;
    float percentLit = 0.0f;
    const float2 offsets[9] =
    {
        float2(-DX,  -DY), float2(0.0f,  -DY), float2(DX,  -DY),
        float2(-DX, 0.0f), float2(0.0f, 0.0f), float2(DX, 0.0f),
        float2(-DX,  +DY), float2(0.0f,  +DY), float2(DX,  +DY)
    };
    
    [unroll]
    for(int i = 0; i < 9; ++i)
    {
        percentLit += gShadowMap.SampleCmpLevelZero(gShadowComSam,
            _shadowPosH.xy + offsets[i], depth).r;
    }

    return percentLit / 9.0f;
}

float3 ClacBumpedNormal(float3 _normalMapSample,
    float3 _unitNormalW, float3 _tangentW)
{
    float3 normalT = 2.0f * _normalMapSample - 1.0f;
    float3 N = _unitNormalW;
    float3 T = normalize(_tangentW - dot(_tangentW, N) * N);
    float3 B = cross(N, T);
    float3x3 TBN = float3x3(T, B, N);

    return mul(normalT, TBN);
}

float4 main(VS_OUTPUT _in) : SV_TARGET
{
	_in.NormalW = normalize(_in.NormalW);
    float3 bumpedNormal = (float3)0.0f;

    if (_in.UseBumped == 1)
    {
        float3 noramlSample = gBumpedMap.Sample(gSampler, _in.TexCoordL).rgb;
        bumpedNormal = ClacBumpedNormal(noramlSample, _in.NormalW, _in.TangentW);
        _in.NormalW = bumpedNormal;
    }

    float3 toEyeW = normalize(gLightInfo[0].gCameraPos - _in.PosW);
    _in.SsaoPosH /= _in.SsaoPosH.w;
    float access = gSsaoMap.SampleLevel(gSampler, _in.SsaoPosH.xy, 0.0f).r;
    float4 ambientL = gAmbient[0].gAmbient * gMaterial[0].gDiffuseAlbedo * access;
    float4 directL = float4(ComputeDirectionalLight(gLights[0], gMaterial[0],
        _in.NormalW, toEyeW), 0.0f);
    float shadow = CalcShadowFactor(_in.ShadowPosH);
    directL.xyz *= shadow;
    
    float4 litColor = ambientL + directL;
    float4 texColor = gDiffuse.Sample(gSampler,_in.TexCoordL);
    litColor *= texColor;
    litColor.a = texColor.a;

    return litColor;
}
