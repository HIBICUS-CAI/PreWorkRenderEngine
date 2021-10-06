struct VS_OUTPUT
{
    float4 PosH : SV_POSITION;
    float3 PosV : POSITION0;
    float2 TexCoordL : TEXCOORD;
};

struct SSAO_INFO
{
    matrix gProj;
    matrix gView;
    matrix gInvProj;
    matrix gProjTex;
    float4 gOffsetVec[14];
    float gOcclusionRadius;
    float gOcclusionFadeStart;
    float gOcclusionFadeEnd;
    float gSurfaceEpsilon;
};

static const float2 gTexCoords[4] = 
{
    float2(0.0f, 1.0f), float2(0.0f, 0.0f),
    float2(1.0f, 0.0f), float2(1.0f, 1.0f)
};

StructuredBuffer<SSAO_INFO> gSsaoInfo : register(t0);

VS_OUTPUT main(uint _vid : SV_VertexID)
{
    VS_OUTPUT output = (VS_OUTPUT)0.0f;
    
    output.TexCoordL = gTexCoords[_vid];
    output.PosH = float4(
        2.0f * output.TexCoordL.x - 1.0f, 1.0f - 2.0f * output.TexCoordL.y, 0.0f, 1.0f);
    float4 ph = mul(output.PosH, gSsaoInfo[0].gInvProj);
    output.PosV = ph.xyz / ph.w;

    return output;
}
