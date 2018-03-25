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

void UMath::CalculateAABB(std::vector<SIMPLE_VERTEX> vertices, std::vector<unsigned int> indices,
	DirectX::XMMATRIX worldmat, AABB &outAABB) {
	// apply world matrix
	DirectX::XMMATRIX transposed = XMMatrixTranspose(worldmat);

	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		DirectX::XMVECTOR temp = XMLoadFloat3(&vertices[i].pos);
		temp.m128_f32[3] = 1;

		DirectX::XMVECTOR pos = DirectX::XMVectorSet(0, 0, 0, 0);
		pos.m128_f32[0] = DirectX::XMVector4Dot(
			temp,
			transposed.r[0]).m128_f32[0];
		pos.m128_f32[1] = DirectX::XMVector4Dot(
			temp,
			transposed.r[1]).m128_f32[0];
		pos.m128_f32[2] = DirectX::XMVector4Dot(
			temp,
			transposed.r[2]).m128_f32[0];

		XMStoreFloat3(&vertices[i].pos, pos);
	}

	outAABB.SetXAxis(DirectX::XMFLOAT2(vertices[0].pos.x, vertices[0].pos.x));
	outAABB.SetYAxis(DirectX::XMFLOAT2(vertices[0].pos.y, vertices[0].pos.y));
	outAABB.SetZAxis(DirectX::XMFLOAT2(vertices[0].pos.z, vertices[0].pos.z));
	for (unsigned int i = 1; i < indices.size(); i++)
	{
		// min
		if (outAABB.GetXAxis().x > vertices[indices[i]].pos.x)
			outAABB.SetXAxis(DirectX::XMFLOAT2(vertices[indices[i]].pos.x, outAABB.GetXAxis().y));
		if (outAABB.GetYAxis().x > vertices[indices[i]].pos.y)
			outAABB.SetYAxis(DirectX::XMFLOAT2(vertices[indices[i]].pos.y, outAABB.GetYAxis().y));
		if (outAABB.GetZAxis().x > vertices[indices[i]].pos.z)
			outAABB.SetZAxis(DirectX::XMFLOAT2(vertices[indices[i]].pos.z, outAABB.GetZAxis().y));

		// max
		if (outAABB.GetXAxis().y < vertices[indices[i]].pos.x)
			outAABB.SetXAxis(DirectX::XMFLOAT2(outAABB.GetXAxis().x, vertices[indices[i]].pos.x));
		if (outAABB.GetYAxis().y < vertices[indices[i]].pos.y)
			outAABB.SetYAxis(DirectX::XMFLOAT2(outAABB.GetYAxis().x, vertices[indices[i]].pos.y));
		if (outAABB.GetZAxis().y < vertices[indices[i]].pos.z)
			outAABB.SetZAxis(DirectX::XMFLOAT2(outAABB.GetZAxis().x, vertices[indices[i]].pos.z));
	}
}