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
    float4 ColorOffset : COLOR;
    float2 TexCoordL : TEXCOORD;
};

struct PROJ
{
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

StructuredBuffer<PROJ> gProj : register(t0);
StructuredBuffer<INSTANCE_DATA> gInstances : register(t1);

VS_OUTPUT main(VS_INPUT _in, uint _instanceID : SV_InstanceID)
{
    VS_OUTPUT _out = (VS_OUTPUT)0;

    _out.PosH = mul(float4(_in.PosL, 1.0f), gInstances[_instanceID].gWorld);
    _out.PosH = mul(_out.PosH, gProj[0].gProjection);
    _out.PosH.z = 0.0f;
    _out.ColorOffset = gInstances[_instanceID].gCustomizedData1;
    _out.TexCoordL = _in.TexCoordL;

    return _out;
}
