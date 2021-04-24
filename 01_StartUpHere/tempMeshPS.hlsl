struct VS_OUTPUT
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 TexCoordL : TEXCOORD;
};

#define SHOWONECOLOR (0.5f)

float4 main(VS_OUTPUT input) : SV_TARGET
{
    return float4(SHOWONECOLOR, SHOWONECOLOR, SHOWONECOLOR, 1.0f);
}