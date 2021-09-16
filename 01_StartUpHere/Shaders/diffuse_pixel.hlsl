struct VS_OUTPUT
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 TexCoordL : TEXCOORD;
};

SamplerState gSampler : register(s0);

Texture2D gDiffuse : register(t0); 

float4 main(VS_OUTPUT input) : SV_TARGET
{
    float4 textureColor = gDiffuse.Sample(gSampler, input.TexCoordL);

	return textureColor;
}
