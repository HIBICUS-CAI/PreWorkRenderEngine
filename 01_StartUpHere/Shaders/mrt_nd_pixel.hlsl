struct VS_OUTPUT
{
    float4 PosH : SV_POSITION;
    float4 DiffuseAlbedo : COLOR0;
    float4 FresnelShiniese : COLOR1;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 TexCoordL : TEXCOORD;
    uint UseBumped : BLENDINDICES;
};

struct PS_OUTPUT
{
    float4 Normal : SV_TARGET0;
};

Texture2D gDiffuse : register(t0);
Texture2D gBumped : register(t1);

SamplerState gLinearSampler : register(s0);

float3 ClacBumpedNormal(float3 _normalMapSample,
    float3 _unitNormalW, float3 _tangentW)
{
    float3 normalT = 2.0f * _normalMapSample - 1.0f;
    float3 N = _unitNormalW;
    float3 T = normalize(_tangentW - dot(_tangentW, N) * N);
    float3 B = cross(N, T);
    float3x3 TBN = float3x3(T, B, N);

    return mul(normalT, TBN);
}

PS_OUTPUT main(VS_OUTPUT _input)
{
    float3 unitNormal = _input.NormalW;
    
    if (_input.UseBumped == 1)
    {
        float3 noramlSample = gBumped.Sample(gLinearSampler, _input.TexCoordL).rgb;
        _input.NormalW = ClacBumpedNormal(noramlSample, unitNormal, _input.TangentW);
    }

    _input.NormalW = normalize(_input.NormalW);

    PS_OUTPUT _out = (PS_OUTPUT)0;
    _out.Normal = float4(_input.NormalW, 0.0f);
    
    return _out;
}
