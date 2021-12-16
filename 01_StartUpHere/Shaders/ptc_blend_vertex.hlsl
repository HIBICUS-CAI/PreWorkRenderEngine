struct VS_OUTPUT
{
    float4 PosH : SV_POSITION;
};

VS_OUTPUT Main(uint vertexId : SV_VertexID)
{
    VS_OUTPUT output = (VS_OUTPUT)0.f;

    float2 corner = float2((vertexId << 1) & 2, vertexId & 2);
    output.PosH = float4(corner * float2(2.f, -2.f) + float2(-1.f, 1.f), 0.f, 1.f);
  
    return output;    
}
