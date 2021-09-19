struct VS_OUTPUT
{
    float4 PosH : SV_POSITION;
    float2 TexCoordL : TEXCOORD;
};

SamplerState gSampler : register(s0);

Texture2D gInput : register(t0);

float4 main(VS_OUTPUT input) : SV_TARGET
{
    float4 textureColor = gInput.Sample(gSampler, input.TexCoordL);

	return textureColor;
}
