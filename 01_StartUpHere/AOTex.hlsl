cbuffer ConstantBuffer : register(b0)
{
    matrix Proj;
    matrix InvProj;
};

struct VS_OUTPUT
{
    float4 PosH : SV_POSITION;
    float3 PosV : POSITION0;
    float2 TexCoordL : TEXCOORD;
};

static const float2 gTexCoords[4] = 
{
    float2(0.0f, 1.0f), float2(0.0f, 0.0f),
    float2(1.0f, 0.0f), float2(1.0f, 1.0f)
};

Texture2D gNormalMap : register(t0);
Texture2D gDepthMap : register(t1);
Texture2D gRenderMap : register(t2);

SamplerState gSamPointClamp : register(s0);
SamplerState gSamLinearClamp : register(s1);
SamplerState gSamDepthMap : register(s2);
SamplerState gSamLinearWrap : register(s3);

VS_OUTPUT VS(uint vid : SV_VertexID)
{
    VS_OUTPUT output = (VS_OUTPUT)0.0f;
    
    output.TexCoordL = gTexCoords[vid];
    output.PosH = float4(2.0f * output.TexCoordL.x - 1.0f, 1.0f - 2.0f * output.TexCoordL.y, 0.0f, 1.0f);
    float4 ph = mul(output.PosH, InvProj);
    output.PosV = ph.xyz / ph.w;

    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    // return float4(0.0f, 1.0f, 1.0f, 1.0f);

    float3 v = gNormalMap.Sample(gSamPointClamp, input.TexCoordL).rgb;
    return float4(v, 1.0f);
}