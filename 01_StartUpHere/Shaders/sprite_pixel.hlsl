struct VS_OUTPUT
{
    float4 PosH : SV_POSITION;
    float4 ColorOffset : COLOR;
    float2 TexCoordL : TEXCOORD;
};

Texture2D gDiffuse : register(t0);
SamplerState gSamLinear : register(s0);

float4 main(VS_OUTPUT input) : SV_TARGET
{
    return gDiffuse.Sample(gSamLinear,input.TexCoordL) * input.ColorOffset;
}
