#ifndef NUM_DIR_LIGHTS
#define NUM_DIR_LIGHTS (1)
#endif
#ifndef NUM_POINT_LIGHTS
#define NUM_POINT_LIGHTS (0)
#endif
#ifndef NUM_SPOT_LIGHTS
#define NUM_SPOT_LIGHTS (0)
#endif

#include "tempLightingUtil.hlsli"

Texture2D CubeTex : register(t0);
SamplerState SamLiner : register(s0);
Texture2D<float> ShadowMap : register(t1);
SamplerState ShadowSamLiner : register(s1);
cbuffer Light : register(b0)
{
    float3 Strength;
    float FalloffStart;
    float3 Direction;
    float FalloffEnd;
    float3 Position;
    float SpotPower;
};
cbuffer AmbientLight : register(b1)
{
    float4 AmbientLight;
};
cbuffer Material : register(b2)
{
    float4 DiffuseAlbedo;
    float3 FresnelR0;
    float Shininess;
};

struct VS_OUTPUT
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION0;
    float4 ShadowPosH : POSITION1;
    float3 NormalW : NORMAL;
    float2 TexCoordL : TEXCOORD;
};

//#define SHOWONECOLOR (0.5f)

float CalcShadowFactor(float4 shadowPosH)
{
    // Complete projection by doing division by w.
    shadowPosH.xyz /= shadowPosH.w;
    shadowPosH.x = 0.5f * shadowPosH.x + 0.5f;
    shadowPosH.y = -0.5f * shadowPosH.y + 0.5f;

    static const float WIDTH = 1280.0f;
    static const float DX = 1.0f / WIDTH;
    static const float HEIGHT = 720.0f;
    static const float DY = 1.0f / HEIGHT;

    // Depth in NDC space.
    float depth = shadowPosH.z;
    float texDepth0 = ShadowMap.Sample(ShadowSamLiner, shadowPosH.xy).r;
    float texDepth1 = ShadowMap.Sample(ShadowSamLiner, shadowPosH.xy + float2(DX, 0)).r;
    float texDepth2 = ShadowMap.Sample(ShadowSamLiner, shadowPosH.xy + float2(0, DY)).r;
    float texDepth3 = ShadowMap.Sample(ShadowSamLiner, shadowPosH.xy + float2(DX, DY)).r;

    float result0 = depth <= texDepth0;
    float result1 = depth <= texDepth1;
    float result2 = depth <= texDepth2;
    float result3 = depth <= texDepth3;

    float2 texelPos = shadowPosH.xy;
    texelPos.x *= WIDTH;
    texelPos.y *= HEIGHT;

    float2 t = frac(texelPos);

    return lerp(lerp(result0, result1, t.x), lerp(result2, result3, t.x), t.y);

    // if(depth > texDepth0)
    // {
    //     return 0.0f;
    // }

    // return 1.0f;
}

float4 main(VS_OUTPUT input) : SV_TARGET
{
#ifdef SHOWONECOLOR

    return float4(SHOWONECOLOR,SHOWONECOLOR,SHOWONECOLOR,1.0f);

#else

    input.NormalW = normalize(input.NormalW);
    float3 toEyeW = normalize(float3(0.0f, 0.0f, -5.0f) - input.PosW);

    // ��ӹ���?
    float4 ambientL = AmbientLight * DiffuseAlbedo;

    // ֱ�ӹ���
    Light l = { Strength,FalloffStart,Direction,FalloffEnd,Position,SpotPower };
    Material mat = { DiffuseAlbedo,FresnelR0,Shininess };
    float4 directL = float4(ComputeSpotLight(l, mat, input.PosW, input.NormalW, toEyeW), 0.0f);
    float shadow = CalcShadowFactor(input.ShadowPosH);
    directL.xyz *= shadow;

    // ����Ч����
    float4 litColor = ambientL + directL;
    float4 texColor = CubeTex.Sample(SamLiner,input.TexCoordL);
    litColor *= texColor;
    litColor.a = texColor.a;

    return litColor;

#endif

}