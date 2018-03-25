struct GPUParticle
{
	float4 worldmat;
	float4 scale;
};

Texture2D random : register(t0);
RWStructuredBuffer<GPUParticle>	OutputBuf : register(u0);

cbuffer PARAMETERS : register(b0)
{
	float4 setSpeed;

	float4 transformPos;
	float3 transformScale;
	float deltaTime;
	uint isOneDirection;
}
SamplerState filters : register(s0); // filter 0 using CLAMP, filter 1 using WRAP

[numthreads(1024, 1, 1)]
void main( uint DTid : SV_DispatchThreadID)
{
	// Generate some random numbers from reading the random texture
	float2 uv = float2(DTid.x / 1024.0, deltaTime);
	float3 randomValues0 = random.SampleLevel(filters, uv, 0).xyz;

	uv = float2(DTid.x + 1 / 1024.0, deltaTime);
	float3 randomValues1 = random.SampleLevel(filters, uv, 0).xyz;

	float x = (isOneDirection ? 1 : (randomValues0.x % 2 ? -1.0f : 1.0f)) * ((float)(randomValues0.y % 100000)) / 100000.0f;
	float y = (isOneDirection ? 1 : (randomValues0.z % 2 ? -1.0f : 1.0f)) * ((float)(randomValues1.x % 100000)) / 100000.0f;
	float z = (isOneDirection ? 1 : (randomValues1.y % 2 ? -1.0f : 1.0f)) * ((float)(randomValues1.z % 100000)) / 100000.0f;
	float3 speed = float3(x, y, z);
	//float4 tempPos = particles.Load(DTid);
	//tempPos += transformPos;
	//tempPos += speed.Load(DTid);
	//tempPos.w = 0;
	//OutputBuf[DTid].worldmat = tempPos;
	//OutputBuf[DTid].scale = float4(transformScale.xy, 0, 0);
}