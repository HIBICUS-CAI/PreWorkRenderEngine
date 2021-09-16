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
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 TexCoordL : TEXCOORD;
};

cbuffer WVP : register(b0)
{
    matrix gWorld;
    matrix gView;
    matrix gProjection;
};

VS_OUTPUT main(VS_INPUT _in)
{
    VS_OUTPUT _out = (VS_OUTPUT)0;

    _out.PosH = mul(float4(_in.PosL, 1.0f), gWorld);
    _out.PosW = _out.PosH.xyz;
    _out.NormalW = mul(_in.NormalL, (float3x3)gWorld);
    _out.TangentW = mul(_in.TangentL, (float3x3)gWorld);
    _out.PosH = mul(_out.PosH, gView);
    _out.PosH = mul(_out.PosH, gProjection);
    _out.TexCoordL = _in.TexCoordL;

    return _out;
}
