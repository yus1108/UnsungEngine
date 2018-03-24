#define MAXVERTS 4

struct INPUT_VERTEX
{
	float4 projectedCoordinate	: SV_POSITION;
	float4 color				: COLOR;
	float2 tex					: TEXCOORD0;
};

// format of output verticies
// these could be going to the
// rasterizer the (SO)stage or both.
struct GSOutput
{
	float4 posH					: SV_POSITION;
	float4 colorOut				: COLOR;
	float2 texOut				: TEXCOORD0;
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

// convert each incoming world-space line segment into a projected triangle. 
[maxvertexcount(MAXVERTS)] // max vertex data to be output (limit 1024 total scalars)
void main(point INPUT_VERTEX input[1] : SV_POSITION, inout TriangleStream< GSOutput > output, uint instanceID : SV_GSInstanceID)
{
	// red green and blue vertex
	GSOutput verts[MAXVERTS] =
	{
		float4(-particles[instanceID].scale.x,-particles[instanceID].scale.y,0,0), float4(0,0,0,1), float2(0,1),
		float4(particles[instanceID].scale.x,-particles[instanceID].scale.y,0,0), float4(0,0,0,1), float2(1,1),
		float4(-particles[instanceID].scale.x,particles[instanceID].scale.y,0,0), float4(0,0,0,1), float2(0,0),
		float4(particles[instanceID].scale.x,particles[instanceID].scale.y,0,0), float4(0,0,0,1), float2(1,0)
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
		verts[i].posH.xyz = verts[i].posH.xyz / verts[i].posH.w;
		verts[i].posH.w = 1;
		output.Append(verts[i]);
	}
	// do not connect to other triangles
	output.RestartStrip();
}
