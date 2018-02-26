struct INPUT_PIXEL
{
	float4 coordinate		: SV_POSITION;
	float2 tex				: TEXCOORD0;
};

texture2D baseTexture : register(t0); // texture to draw

SamplerState filters : register(s0); // filter 0 using CLAMP, filter 1 using WRAP
float4 main(INPUT_PIXEL input) : SV_TARGET
{
	float4 color = baseTexture.Sample(filters, input.tex); // get base color
	return color; // return a transition based on the detail alpha
}

