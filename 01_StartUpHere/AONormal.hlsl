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
    float3 NormalV : NORMAL;
    float2 TexCoordL : TEXCOORD;
};

Texture2D NullTex : register(t0);

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0.0f;
    output.PosH = mul(float4(input.PosL, 1.0f), World);
    output.NormalV = mul(input.NormalL, (float3x3)World);
    output.NormalV = mul(output.NormalV, (float3x3)View);
    output.PosH = mul(output.PosH, View);
    output.PosH = mul(output.PosH, Projection);
    output.TexCoordL = input.TexCoordL;
    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    input.NormalV = normalize(input.NormalV);
    return float4(input.NormalV, 0.0f);
}