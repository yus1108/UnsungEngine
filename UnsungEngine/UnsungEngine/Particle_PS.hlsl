struct INPUT_PIXEL
{
	float4 coordinate		: SV_POSITION;
	float4 color			: COLOR;
};

float4 main(INPUT_PIXEL input) : SV_TARGET
{
	return input.color; // return a transition based on the detail alpha
}

