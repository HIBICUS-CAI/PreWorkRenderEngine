struct VS_OUTPUT
{
    float4 PosH : SV_POSITION;
    float3 NormalV : NORMAL;
    float3 TangentW : TANGENT;
    float2 TexCoordL : TEXCOORD;
};

struct VIEWPROJ
{
    matrix gView;
    matrix gProjection;
};

StructuredBuffer<VIEWPROJ> gViewProj : register(t0);

float4 main(VS_OUTPUT _input) : SV_TARGET
{
    _input.NormalV = mul(_input.NormalV, (float3x3)gViewProj[0].gView);
    _input.NormalV = normalize(_input.NormalV);
    
    return float4(_input.NormalV, 0.0f);
}
