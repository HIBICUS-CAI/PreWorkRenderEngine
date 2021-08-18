cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
    matrix ShadowView;
    matrix ShadowProj;
};
struct VS_INPUT
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float2 TexCoordL : TEXCOORD;
};
struct VS_OUTPUT
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION0;
    float4 ShadowPosH : POSITION1;
    float3 NormalW : NORMAL;
    float2 TexCoordL : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.PosH = mul(float4(input.PosL, 1.0f), World);
    output.PosW = output.PosH.xyz;
    output.NormalW = mul(input.NormalL, (float3x3)World);
    output.PosH = mul(output.PosH, View);
    output.PosH = mul(output.PosH, Projection);
    output.ShadowPosH = mul(float4(output.PosW, 1.0f), ShadowView);
    output.ShadowPosH = mul(output.ShadowPosH, ShadowProj);
    output.TexCoordL = input.TexCoordL;
    return output;
}