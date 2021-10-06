struct VS_OUTPUT
{
    float4 PosH : SV_POSITION;
    float2 TexCoordL : TEXCOORD;
};

float4 main(VS_OUTPUT _input) : SV_TARGET
{
    return float4(0.5f, 0.5f, 0.5f, 1.0f);
}
