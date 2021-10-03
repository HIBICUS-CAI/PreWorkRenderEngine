struct VS_OUTPUT
{
    float4 PosH : SV_POSITION;
    float3 NormalV : NORMAL;
    float3 TangentW : TANGENT;
    float2 TexCoordL : TEXCOORD;
};

struct PS_OUTPUT
{
    float4 Diffuse : SV_TARGET0;
    float4 Normal : SV_TARGET1;
};

struct VIEWPROJ
{
    matrix gView;
    matrix gProjection;
};

StructuredBuffer<VIEWPROJ> gViewProj : register(t0);
Texture2D gDiffuse : register(t1);

SamplerState gLinearSampler : register(s0);

PS_OUTPUT main(VS_OUTPUT _input)
{
    _input.NormalV = mul(_input.NormalV, (float3x3)gViewProj[0].gView);
    _input.NormalV = normalize(_input.NormalV);

    PS_OUTPUT _out = (PS_OUTPUT)0;
    _out.Normal = float4(_input.NormalV, 0.0f);
    _out.Diffuse = gDiffuse.Sample(gLinearSampler,_input.TexCoordL);
    
    return _out;
}
