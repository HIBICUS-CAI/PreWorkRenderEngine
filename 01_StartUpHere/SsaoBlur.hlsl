[numthreads(1, 1, 1)]
void main(int3 dispatchThreadId : SV_DispatchThreadID)
{
    int a = 1;
    int b = 2;
    int c = a + b;
}