struct INPUT_PIXEL
{
	float4 coordinate		: SV_POSITION;
	float4 color			: COLOR;
	float2 tex				: TEXCOORD0;
};

texture2D baseTexture : register(t0); // texture to draw
SamplerState filters : register(s0); // filter 0 using CLAMP, filter 1 using WRAP
float4 main(INPUT_PIXEL input) : SV_TARGET
{
	return baseTexture.Sample(filters, input.tex) * input.color;
}

