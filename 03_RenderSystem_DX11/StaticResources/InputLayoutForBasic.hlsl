struct BASIC_INPUT
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float2 TexCoordL : TEXCOORD;
};

float4 main(BASIC_INPUT input) : SV_Position
{
    return float4(1, 1, 1, 1);
}
