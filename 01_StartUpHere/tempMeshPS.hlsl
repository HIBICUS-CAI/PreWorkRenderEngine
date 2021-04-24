struct VS_OUTPUT
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 TexCoordL : TEXCOORD;
};

Texture2D g_DiffuseTexture : register(t0);
SamplerState g_TexSampler : register(s0);

#define SHOWONECOLOR (0.5f)

float4 main(VS_OUTPUT input) : SV_TARGET
{
    float4 textureColor = g_DiffuseTexture.Sample(g_TexSampler, input.TexCoordL);

	return textureColor;
}