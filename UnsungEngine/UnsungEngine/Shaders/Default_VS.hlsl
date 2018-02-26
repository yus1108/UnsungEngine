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

OUTPUT_VERTEX main(INPUT_VERTEX fromVertexBuffer)
{
	OUTPUT_VERTEX sendToRasterizer = (OUTPUT_VERTEX)0;

	sendToRasterizer.projectedCoordinate = float4(fromVertexBuffer.coordinate, 1);
	sendToRasterizer.tex = fromVertexBuffer.tex;

	return sendToRasterizer;
}