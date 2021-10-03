struct VS_INPUT
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float3 TangentL : TANGENT;
    float2 TexCoordL : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION0;
    float4 ShadowPosH : POSITION1;
    float4 SsaoPosH : POSITION2;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 TexCoordL : TEXCOORD;
    uint UseBumped : BLENDINDICES;
};

struct VIEWPROJ
{
    matrix gView;
    matrix gProjection;
};

struct MATERIAL
{
    float4 gDiffuseAlbedo;
    float3 gFresnelR0;
    float gShininess;
};

struct INSTANCE_DATA
{
    matrix gWorld;
    MATERIAL gMaterial;
    float4 gCustomizedData1;
    float4 gCustomizedData2;
};

struct SHADOW_INFO
{
    matrix gShadowViewMat;
    matrix gShadowProjMat;
    matrix gSSAOMat;
};

StructuredBuffer<VIEWPROJ> gViewProj : register(t0);
StructuredBuffer<INSTANCE_DATA> gInstances : register(t1);
StructuredBuffer<SHADOW_INFO> gShadowInfo : register(t2);

VS_OUTPUT main(VS_INPUT _in, uint _instanceID : SV_InstanceID)
{
    VS_OUTPUT _out = (VS_OUTPUT)0;

    _out.PosH = mul(float4(_in.PosL, 1.0f), gInstances[_instanceID].gWorld);
    _out.PosW = _out.PosH.xyz;
    _out.NormalW = mul(_in.NormalL, (float3x3)gInstances[_instanceID].gWorld);
    _out.TangentW = mul(_in.TangentL, (float3x3)gInstances[_instanceID].gWorld);
    _out.PosH = mul(_out.PosH, gViewProj[0].gView);
    _out.PosH = mul(_out.PosH, gViewProj[0].gProjection);
    _out.ShadowPosH = mul(float4(_out.PosW, 1.0f), gShadowInfo[0].gShadowViewMat);
    _out.ShadowPosH = mul(_out.ShadowPosH, gShadowInfo[0].gShadowProjMat);
    _out.SsaoPosH = mul(float4(_out.PosW, 1.0f), gShadowInfo[0].gSSAOMat);
    _out.TexCoordL = _in.TexCoordL;

    if (gInstances[_instanceID].gCustomizedData1.x > 0.0f)
    {
        _out.UseBumped = 1;
    }
    else
    {
        _out.UseBumped = 0;
    }

    return _out;
}
