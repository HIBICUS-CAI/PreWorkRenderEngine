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
SamplerComparisonState ShadowComSam : register(s2);
Texture2D SsaoMap : register(t2);

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
    float4 SsaoPosH : POSITION2;
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

    uint width, height, numMips;
    ShadowMap.GetDimensions(0, width, height, numMips);

    const float WIDTH = (float)width;
    const float DX = 1.0f / WIDTH;
    const float HEIGHT = (float)height;
    const float DY = 1.0f / HEIGHT;

    // Depth in NDC space.
    // float depth = shadowPosH.z;
    // float texDepth0 = ShadowMap.Sample(ShadowSamLiner, shadowPosH.xy).r;
    // float texDepth1 = ShadowMap.Sample(ShadowSamLiner, shadowPosH.xy + float2(DX, 0)).r;
    // float texDepth2 = ShadowMap.Sample(ShadowSamLiner, shadowPosH.xy + float2(0, DY)).r;
    // float texDepth3 = ShadowMap.Sample(ShadowSamLiner, shadowPosH.xy + float2(DX, DY)).r;

    // float result0 = depth <= texDepth0;
    // float result1 = depth <= texDepth1;
    // float result2 = depth <= texDepth2;
    // float result3 = depth <= texDepth3;

    // float2 texelPos = shadowPosH.xy;
    // texelPos.x *= WIDTH;
    // texelPos.y *= HEIGHT;

    // float2 t = frac(texelPos);

    // return lerp(lerp(result0, result1, t.x), lerp(result2, result3, t.x), t.y);

    float depth = shadowPosH.z;
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
        percentLit += ShadowMap.SampleCmpLevelZero(ShadowComSam,
            shadowPosH.xy + offsets[i], depth).r;
    }

    return percentLit / 9.0f;

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
    input.SsaoPosH /= input.SsaoPosH.w;
    float access = SsaoMap.SampleLevel(SamLiner, input.SsaoPosH.xy, 0.0f).r;
    float4 ambientL = AmbientLight * DiffuseAlbedo * access;

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