struct INPUT_VERTEX
{
	float3 coordinate	: POSITION;
	float4 color		: COLOR;
	float2 tex			: TEXCOORD0;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
	float3 binormal		: BINORMAL;
	float4 weights		: BLENDWEIGHT;
	uint4  joints		: BLENDINDICES;
};

struct OUTPUT_VERTEX
{
	float4 projectedCoordinate	: SV_POSITION;
	float4 colorOut				: COLOR;
	float2 tex					: TEXCOORD0;
	float3 normal				: NORMAL;
	float3 worldPosition		: POSITION;
	float3 tangent				: TANGENT;
	float3 binormal				: BINORMAL;
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
	sendToRasterizer.worldPosition = localH.xyz;

	sendToRasterizer.normal = mul(fromVertexBuffer.normal, (float3x3)worldMatrix);
	sendToRasterizer.normal = normalize(sendToRasterizer.normal);
	sendToRasterizer.tangent = mul(fromVertexBuffer.tangent, (float3x3)worldMatrix);
	sendToRasterizer.tangent = normalize(sendToRasterizer.tangent);
	sendToRasterizer.binormal = mul(fromVertexBuffer.binormal, (float3x3)worldMatrix);
	sendToRasterizer.binormal = normalize(sendToRasterizer.binormal);

	// Move into view space, then projection space
	localH = mul(localH, viewMatrix);
	localH = mul(localH, projectionMatrix);
	sendToRasterizer.projectedCoordinate = localH;
	sendToRasterizer.tex = fromVertexBuffer.tex;
	sendToRasterizer.colorOut = fromVertexBuffer.color;

	return sendToRasterizer;
}