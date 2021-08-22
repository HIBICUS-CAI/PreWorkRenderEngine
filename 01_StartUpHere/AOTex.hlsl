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

static const float2 gTexCoords[6] = 
{
    float2(0.0f, 1.0f), float2(0.0f, 0.0f),
    float2(1.0f, 0.0f), float2(0.0f, 1.0f),
    float2(1.0f, 0.0f), float2(1.0f, 1.0f)
};

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
    return float4(0.0f, 1.0f, 1.0f, 1.0f);
}