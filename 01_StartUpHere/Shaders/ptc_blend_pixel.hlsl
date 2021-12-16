// TEMP-----------------------------
static const uint gScreenWidth = 1280;
static const uint gScreenHeight = 720;
// TEMP-----------------------------

struct VS_OUTPUT
{
    float4 PosH : SV_POSITION;
};

Buffer<float4> gParticleRenderBuffer : register(t0);

float4 Main(float4 posH : SV_POSITION) : SV_Target
{
	float4 color = (float4)0.f;

	float x = posH.x - (gScreenWidth / 2);
	float y = posH.y;
	uint pixelIndex = x + (y * gScreenWidth);
	float4 particleValue = gParticleRenderBuffer.Load( pixelIndex );
	color = particleValue;

	return color;
}
