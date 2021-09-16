struct BASIC_INPUT
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float4 Color : COLOR;
};

float4 main(BASIC_INPUT input) : SV_Position
{
    return float4(1, 1, 1, 1);
}
