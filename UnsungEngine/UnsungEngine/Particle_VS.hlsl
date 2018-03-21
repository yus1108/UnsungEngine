struct INPUT_VERTEX
{
	float3 coordinate	: POSITION;
	float4 color		: COLOR;
};

struct OUTPUT_VERTEX
{
	float4 projectedCoordinate	: SV_POSITION;
	float4 colorOut				: COLOR;
};

cbuffer WORLD : register(b0)
{
	float4x4 worldMatrix;
}
cbuffer SCENE : register(b1)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
}

OUTPUT_VERTEX main(INPUT_VERTEX fromVertexBuffer)
{
	OUTPUT_VERTEX sendToRasterizer = (OUTPUT_VERTEX)0;

	float4 localH = float4(fromVertexBuffer.coordinate, 1);
	// move local space vertex from vertex buffer into world space.
	localH = mul(localH, worldMatrix);
	localH = mul(localH, viewMatrix);
	sendToRasterizer.projectedCoordinate = localH;
	sendToRasterizer.colorOut = fromVertexBuffer.color;

	return sendToRasterizer;
}