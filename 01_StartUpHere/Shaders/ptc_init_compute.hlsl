AppendStructuredBuffer<uint> g_DeadListToAddTo : register(u0);

[numthreads(256, 1, 1)]
void Main(uint3 id : SV_DispatchThreadID)
{
    g_DeadListToAddTo.Append(id.x);
}
