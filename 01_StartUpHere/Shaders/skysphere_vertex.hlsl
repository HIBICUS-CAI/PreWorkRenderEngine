struct VS_INPUT
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float3 TangentL : TANGENT;
    float2 TexCoordL : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 PosH : SV_POSITION;
    float3 PosL : POSITION;
};

struct SKYSPHERE_INFO
{
    matrix gWorld;
    matrix gView;
    matrix gProj;
    float3 gEyePos;
    float gPad;
};

StructuredBuffer<SKYSPHERE_INFO> gSkySphereInfo : register(t0);

VS_OUTPUT main(VS_INPUT _input)
{
    VS_OUTPUT output = (VS_OUTPUT)0.0f;

    output.PosL = _input.PosL;
    // float4 posW = float4(_input.PosL, 1.0f);
    float4 posW = mul(float4(_input.PosL, 1.0f), gSkySphereInfo[0].gWorld);
    posW.xyz += gSkySphereInfo[0].gEyePos;

    output.PosH = mul(posW, gSkySphereInfo[0].gView);
    output.PosH = mul(output.PosH, gSkySphereInfo[0].gProj);
    output.PosH = output.PosH.xyww;

    return output;
}
