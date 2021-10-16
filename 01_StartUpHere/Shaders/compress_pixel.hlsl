struct VS_OUTPUT
{
    float4 PosH : SV_POSITION;
    float2 TexCoordL : TEXCOORD;
};

SamplerState gSampler : register(s0);
Texture2D gSourceTex : register(t0);

float4 main(VS_OUTPUT _in) : SV_TARGET
{
    return gSourceTex.SampleLevel(gSampler, _in.TexCoordL * 2.0, 0.0f);
}
