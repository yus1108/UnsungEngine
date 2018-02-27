#define MAXVERTS 4

struct INPUT_VERTEX
{
	float4 projectedCoordinate	: SV_POSITION;
	float2 tex					: TEXCOORD0;
};

// format of output verticies
// these could be going to the
// rasterizer the (SO)stage or both.
struct GSOutput
{
	float4 posH					: SV_POSITION;
	float2 texOut				: TEXCOORD0;
};

cbuffer SIZE : register(b1)
{
	float4 size;
}

// convert each incoming world-space line segment into a projected triangle. 
[maxvertexcount(MAXVERTS)] // max vertex data to be output (limit 1024 total scalars)
void main(point INPUT_VERTEX input[1] : SV_POSITION, inout TriangleStream< GSOutput > output)
{
	// red green and blue vertex
	GSOutput verts[MAXVERTS] =
	{
		float4(-1,-1,0,1), float2(0,1),
		float4(-1,1,0,1), float2(0,0),
		float4(1,-1,0,1), float2(1,1),
		float4(1,1,0,1), float2(1,0)
	};

	verts[0].posH = input[0].projectedCoordinate;
	verts[0].posH += float4(size.x, size.y, 0, 0);

	verts[1].posH = input[0].projectedCoordinate;
	verts[1].posH += float4(size.x, size.w, 0, 0);

	verts[2].posH = input[0].projectedCoordinate;
	verts[2].posH += float4(size.z, size.y, 0, 0);

	verts[3].posH = input[0].projectedCoordinate;
	verts[3].posH += float4(size.z, size.w, 0, 0);

	// prep triangle for rasterization
	for (uint i = 0; i < MAXVERTS; ++i)
	{
		// send verts to the rasterizer
		output.Append(verts[i]);
	}
	// do not connect to other triangles
	output.RestartStrip();
}
