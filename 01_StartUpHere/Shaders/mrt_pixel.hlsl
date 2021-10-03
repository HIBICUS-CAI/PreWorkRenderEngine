struct VS_OUTPUT
{
    float4 PosH : SV_POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 TexCoordL : TEXCOORD;
    uint UseBumped : BLENDINDICES;
};

struct PS_OUTPUT
{
    float4 Diffuse : SV_TARGET0;
    float4 Normal : SV_TARGET1;
    float4 BumpedNormal : SV_TARGET2;
};

struct VIEWPROJ
{
    matrix gView;
    matrix gProjection;
};

StructuredBuffer<VIEWPROJ> gViewProj : register(t0);
Texture2D gDiffuse : register(t1);
Texture2D gBumped : register(t2);

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

    _input.NormalW = mul(_input.NormalW, (float3x3)gViewProj[0].gView);
    _input.NormalW = normalize(_input.NormalW);

    unitNormal = mul(unitNormal, (float3x3)gViewProj[0].gView);
    unitNormal = normalize(unitNormal);

    PS_OUTPUT _out = (PS_OUTPUT)0;
    _out.Normal = float4(unitNormal, 0.0f);
    _out.BumpedNormal = float4(_input.NormalW, 0.0f);
    _out.Diffuse = gDiffuse.Sample(gLinearSampler,_input.TexCoordL);
    
    return _out;
}
