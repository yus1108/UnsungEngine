#define MAXVERTS 4

struct INPUT_VERTEX
{
	float4 projectedCoordinate	: SV_POSITION;
	float4 color				: COLOR;
};

// format of output verticies
// these could be going to the
// rasterizer the (SO)stage or both.
struct GSOutput
{
	float4 posH					: SV_POSITION;
	float4 colorOut				: COLOR;
};

cbuffer SCENE : register(b1)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
}

// convert each incoming world-space line segment into a projected triangle. 
[maxvertexcount(MAXVERTS)] // max vertex data to be output (limit 1024 total scalars)
void main(point INPUT_VERTEX input[1] : SV_POSITION, inout TriangleStream< GSOutput > output)
{
	// red green and blue vertex
	GSOutput verts[MAXVERTS] =
	{
		float4(-0.01f,-0.01f,0,0), float4(0,0,0,1),
		float4(0.01f,-0.01f,0,0), float4(0,0,0,1),
		float4(-0.01f,0.01f,0,0), float4(0,0,0,1),
		float4(0.01f,0.01f,0,0), float4(0,0,0,1)
	};

	verts[0].posH += input[0].projectedCoordinate;
	verts[0].colorOut = input[0].color;
	verts[1].posH += input[0].projectedCoordinate;
	verts[1].colorOut = input[0].color;
	verts[2].posH += input[0].projectedCoordinate;
	verts[2].colorOut = input[0].color;
	verts[3].posH += input[0].projectedCoordinate;
	verts[3].colorOut = input[0].color;

	// prep triangle for rasterization
	for (uint i = 0; i < MAXVERTS; ++i)
	{
		// send verts to the rasterizer
		verts[i].posH = mul(verts[i].posH, projectionMatrix);
		verts[i].posH.z = verts[i].posH.z / verts[i].posH.w;
		verts[i].posH.w = 1;
		output.Append(verts[i]);
	}
	// do not connect to other triangles
	output.RestartStrip();
}
