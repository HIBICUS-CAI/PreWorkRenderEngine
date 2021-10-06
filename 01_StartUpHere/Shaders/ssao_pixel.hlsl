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

static const int gSampleCount = 14;

StructuredBuffer<SSAO_INFO> gSsaoInfo : register(t0);
Texture2D gNormalMap : register(t1);
Texture2D gDepthMap : register(t2);
Texture2D gRandomMap : register(t3);

SamplerState gSamPointClamp : register(s0);
SamplerState gSamLinearClamp : register(s1);
SamplerState gSamDepthMap : register(s2);
SamplerState gSamLinearWrap : register(s3);

float NdcDepthToViewDepth(float _z_ndc)
{
    float viewZ = gSsaoInfo[0].gProj[3][2] / (_z_ndc - gSsaoInfo[0].gProj[2][2]);
    return viewZ;
}

float Occlusion(float _deltaZ)
{
    float occlusion = 0.0f;
    if (_deltaZ > gSsaoInfo[0].gSurfaceEpsilon)
    {
        float fadeLen = gSsaoInfo[0].gOcclusionFadeEnd - gSsaoInfo[0].gOcclusionFadeStart;
        occlusion = saturate((gSsaoInfo[0].gOcclusionFadeEnd - _deltaZ) / fadeLen);
    }
    return occlusion;
}

float4 main(VS_OUTPUT _input) : SV_TARGET
{
    float3 n = normalize(gNormalMap.SampleLevel(gSamPointClamp, _input.TexCoordL, 0.0f).xyz);
    n = normalize(mul(n, (float3x3)gSsaoInfo[0].gView));
    float pz = gDepthMap.SampleLevel(gSamDepthMap, _input.TexCoordL, 0.0f).r;
    pz = NdcDepthToViewDepth(pz);

    float3 p = (pz / _input.PosV.z) * _input.PosV;
    
    float3 randVec = gRandomMap.SampleLevel(gSamLinearWrap, 5.0f * _input.TexCoordL, 0.0f).rgb;
    randVec = 2.0f * randVec - 1.0f;

    float occlusionSum = 0.0f;

    for (int i = 0; i < gSampleCount; ++i)
    {
        float3 offset = reflect(gSsaoInfo[0].gOffsetVec[i].xyz, randVec);

        float flip = sign(dot(offset, n));

        float3 q = p + flip * gSsaoInfo[0].gOcclusionRadius * offset;

        float4 projQ = mul(float4(q, 1.0f), gSsaoInfo[0].gProjTex);
        projQ /= projQ.w;

        float rz = gDepthMap.SampleLevel(gSamDepthMap, projQ.xy, 0.0f).r;
        rz = NdcDepthToViewDepth(rz);

        float3 r = (rz / q.z) * q;

        float deltaZ = p.z - r.z;
        float dp = max(dot(n, normalize(r - p)), 0.0f);

        float occlusion = dp * Occlusion(deltaZ);

        occlusionSum += occlusion;
    }

    occlusionSum /= gSampleCount;

    float accsee = 1.0f - occlusionSum;

    return saturate(pow(accsee, 5.0f));
}
