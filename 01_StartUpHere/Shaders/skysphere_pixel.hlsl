struct VS_OUTPUT
{
    float4 PosH : SV_POSITION;
    float3 PosL : POSITION;
};

TextureCube gCubeMap : register(t0);

SamplerState gSamLinearWrap : register(s0);

float4 main(VS_OUTPUT _input) : SV_TARGET
{
    return gCubeMap.Sample(gSamLinearWrap, _input.PosL);
}
