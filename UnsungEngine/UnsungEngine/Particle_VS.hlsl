struct INPUT_VERTEX
{
	float3 coordinate	: POSITION;
	float4 color		: COLOR;
	float2 tex			: TEXCOORD0;
};

struct OUTPUT_VERTEX
{
	float4 projectedCoordinate	: SV_POSITION;
	float4 colorOut				: COLOR;
	float2 tex					: TEXCOORD0;
};

struct Particle {
	float4 worldMatrix;
	float4 scale;
};
cbuffer WORLD : register(b0)
{
	Particle particles[1000];
}
cbuffer SCENE : register(b1)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
}

OUTPUT_VERTEX main(INPUT_VERTEX fromVertexBuffer, uint instanceID : SV_InstanceID)
{
	OUTPUT_VERTEX sendToRasterizer = (OUTPUT_VERTEX)0;

	float4 localH = float4(fromVertexBuffer.coordinate, 1);
	localH += particles[instanceID].worldMatrix;
	localH = mul(localH, viewMatrix);
	sendToRasterizer.projectedCoordinate = localH;
	sendToRasterizer.colorOut = fromVertexBuffer.color;

	return sendToRasterizer;
}