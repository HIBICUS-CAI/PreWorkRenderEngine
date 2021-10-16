struct VS_OUTPUT
{
    float4 PosH : SV_POSITION;
    float4 LightColorW : COLOR;
};

float4 main(VS_OUTPUT _input) : SV_TARGET
{
    return _input.LightColorW;
}
