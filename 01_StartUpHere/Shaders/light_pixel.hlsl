#include "light_helper.hlsli"

struct VS_OUTPUT
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 TexCoordL : TEXCOORD;
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

StructuredBuffer<AMBIENT> gAmbient : register(t0);
StructuredBuffer<LIGHT_INFO> gLightInfo : register(t1);
StructuredBuffer<MATERIAL> gMaterial : register(t2);
StructuredBuffer<LIGHT> gLights : register(t3);

Texture2D gDiffuse : register(t4);

float4 main(VS_OUTPUT _in) : SV_TARGET
{
	_in.NormalW = normalize(_in.NormalW);
    float3 toEyeW = normalize(gLightInfo[0].gCameraPos - _in.PosW);
    float4 ambientL = gAmbient[0].gAmbient * gMaterial[0].gDiffuseAlbedo;
    float4 directL = float4(ComputeDirectionalLight(gLights[0], gMaterial[0],
        _in.NormalW, toEyeW), 0.0f);
    
    float4 litColor = ambientL + directL;
    float4 texColor = gDiffuse.Sample(gSampler,_in.TexCoordL);
    litColor *= texColor;
    litColor.a = texColor.a;

    return litColor;
}
