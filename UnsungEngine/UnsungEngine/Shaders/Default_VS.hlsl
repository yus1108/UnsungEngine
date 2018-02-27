struct INPUT_VERTEX
{
	float3 coordinate	: POSITION;
	float2 tex			: TEXCOORD0;
};

struct OUTPUT_VERTEX
{
	float4 projectedCoordinate	: SV_POSITION;
	float2 tex					: TEXCOORD0;
};

cbuffer SCREEN : register(b0)
{
	float4 screenPos;
}

OUTPUT_VERTEX main(INPUT_VERTEX fromVertexBuffer)
{
	OUTPUT_VERTEX sendToRasterizer = (OUTPUT_VERTEX)0;

	sendToRasterizer.projectedCoordinate = screenPos;
	sendToRasterizer.tex = fromVertexBuffer.tex;

	return sendToRasterizer;
}