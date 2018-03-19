#include "stdafx.h"
#include "UMath.h"

float UMath::Convert_DegreeToRad(float degree)
{
	return degree / 180.0f * 3.14159f;
}

double UMath::Convert_DegreeToRad(double degree)
{
	return degree / 180.0 * 3.14159265359;
}

DirectX::XMFLOAT2 UMath::ConvertPixelToNDC(POINT pos_in, HWND * window)
{
	DirectX::XMFLOAT2 pos;
	RECT windowSize;
	RECT clientSize;
	GetWindowRect(*window, &windowSize);
	GetClientRect(*window, &clientSize);
	float offset = (float)((windowSize.right - windowSize.left) - (clientSize.right - clientSize.left)) / 2.0f;
	float offsetY = (float)((windowSize.bottom - windowSize.top) - (clientSize.bottom - clientSize.top)) - offset;
	float horizonStart = (float)windowSize.left + offset;
	float horizonEnd = (float)windowSize.right - offset;
	float verticalStart = (float)windowSize.top + offsetY;
	float verticalEnd = (float)windowSize.bottom - offset;
	pos.x = ((float)pos_in.x - horizonStart) / (horizonEnd - horizonStart);
	pos.x -= 0.5f;
	pos.x *= 2.0f;
	pos.y = ((float)pos_in.y - verticalStart) / (verticalEnd - verticalStart);
	pos.y -= 0.5f;
	pos.y = -pos.y * 2.0f;

	return pos;
}
