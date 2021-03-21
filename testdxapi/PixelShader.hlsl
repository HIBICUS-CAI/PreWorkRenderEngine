#ifndef NUM_DIR_LIGHTS
#define NUM_DIR_LIGHTS (1)
#endif
#ifndef NUM_POINT_LIGHTS
#define NUM_POINT_LIGHTS (0)
#endif
#ifndef NUM_SPOT_LIGHTS
#define NUM_SPOT_LIGHTS (0)
#endif

#include "LightingUtil.hlsli"

cbuffer Light : register(b0)
{
    float3 Strength;
    float FalloffStart;
    float3 Direction;
    float FalloffEnd;
    float3 Position;
    float SpotPower;
}
cbuffer AmbientLight : register(b1)
{
    float4 AmbientLight;
}
cbuffer Material : register(b2)
{
    float4 DiffuseAlbedo;
    float3 FresnelR0;
    float Shininess;
}

struct VS_OUTPUT
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
};

//#define SHOWONECOLOR (0.5f)

float4 main(VS_OUTPUT input) : SV_TARGET
{
#ifdef SHOWONECOLOR

    return float4(SHOWONECOLOR,SHOWONECOLOR,SHOWONECOLOR,1.0f);

#else

    input.NormalW = normalize(input.NormalW);
    float3 toEyeW = normalize(float3(0.0f, 0.0f, -5.0f) - input.PosW);

    // 间接光照
    float4 ambientL = AmbientLight * DiffuseAlbedo;

    // 直接光照
    Light l = { Strength,FalloffStart,Direction,FalloffEnd,Position,SpotPower };
    Material mat = { DiffuseAlbedo,FresnelR0,Shininess };
    float4 directL = float4(ComputeSpotLight(l, mat, input.PosW, input.NormalW, toEyeW), 0.0f);
    
    // 光照效果和
    float4 litColor = ambientL + directL;
    litColor.a = DiffuseAlbedo.a;

    return litColor;

#endif

}