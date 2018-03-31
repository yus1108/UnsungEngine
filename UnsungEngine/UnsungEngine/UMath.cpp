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

bool UMath::CollisionTest(AABB a, AABB b) {
	DirectX::XMFLOAT2 aX = a.GetXAxis();
	DirectX::XMFLOAT2 aY = a.GetYAxis();
	DirectX::XMFLOAT2 aZ = a.GetZAxis();
	DirectX::XMFLOAT2 bX = b.GetXAxis();
	DirectX::XMFLOAT2 bY = b.GetYAxis();
	DirectX::XMFLOAT2 bZ = b.GetZAxis();

	if (aX.y < bX.x || aX.x > bX.y) return false;
	if (aY.y < bY.x || aY.x > bY.y) return false;
	if (aZ.y < bZ.x || aZ.x > bZ.y) return false;

	return true;
}
bool UMath::CollisionTest(AABB a, OOBB b) {
	DirectX::XMMATRIX tempMatrix = DirectX::XMMatrixIdentity();
	OOBB aOOBB = OOBB(&tempMatrix, a.GetXAxis(), a.GetYAxis(), a.GetZAxis());
	return CollisionTest(&aOOBB, &b);
}
bool UMath::CollisionTest(OOBB * obb1, OOBB * obb2) {
	using namespace DirectX;
	XMVECTOR vertices1[8], vertices2[8];
	float min1, min2, max1, max2;
	// xaxis
	min1 = min2 = FLT_MAX;
	max1 = max2 = -FLT_MAX;
	for (int i = 0; i < 8; i++)
	{
		vertices1[i] = XMVector4Transform(obb1->GetVertex(i), obb1->GetWorldMat());
		vertices2[i] = XMVector4Transform(obb2->GetVertex(i), obb2->GetWorldMat());

		XMVECTOR dist1 = XMVector3Dot(XMVector3Normalize(obb1->GetWorldMatAddr()->r[0]), vertices1[i]);
		XMVECTOR dist2 = XMVector3Dot(XMVector3Normalize(obb1->GetWorldMatAddr()->r[0]), vertices2[i]);
		if (min1 > dist1.m128_f32[0])
			min1 = dist1.m128_f32[0];
		if (min2 > dist2.m128_f32[0])
			min2 = dist2.m128_f32[0];
		if (max1 < dist1.m128_f32[0])
			max1 = dist1.m128_f32[0];
		if (max2 < dist2.m128_f32[0])
			max2 = dist2.m128_f32[0];
	}
	float center1 = (max1 + min1) / 2.0f;
	float center2 = (max2 + min2) / 2.0f;
	if ((max2 - center2) + (max1 - center1) < abs(center2 - center1))
		return false;

	// yaxis
	min1 = max1 = XMVector3Dot(XMVector3Normalize(obb1->GetWorldMatAddr()->r[1]), vertices1[0]).m128_f32[0];
	min2 = max2 = XMVector3Dot(XMVector3Normalize(obb1->GetWorldMatAddr()->r[1]), vertices2[0]).m128_f32[0];
	for (int i = 1; i < 8; i++)
	{
		XMVECTOR dist1 = XMVector3Dot(XMVector3Normalize(obb1->GetWorldMatAddr()->r[1]), vertices1[i]);
		XMVECTOR dist2 = XMVector3Dot(XMVector3Normalize(obb1->GetWorldMatAddr()->r[1]), vertices2[i]);
		if (min1 > dist1.m128_f32[0])
			min1 = dist1.m128_f32[0];
		if (min2 > dist2.m128_f32[0])
			min2 = dist2.m128_f32[0];
		if (max1 < dist1.m128_f32[0])
			max1 = dist1.m128_f32[0];
		if (max2 < dist2.m128_f32[0])
			max2 = dist2.m128_f32[0];
	}
	center1 = (max1 + min1) / 2.0f;
	center2 = (max2 + min2) / 2.0f;
	if ((max2 - center2) + (max1 - center1) < abs(center2 - center1))
		return false;

	// zaxis
	min1 = max1 = XMVector3Dot(XMVector3Normalize(obb1->GetWorldMatAddr()->r[2]), vertices1[0]).m128_f32[0];
	min2 = max2 = XMVector3Dot(XMVector3Normalize(obb1->GetWorldMatAddr()->r[2]), vertices2[0]).m128_f32[0];
	for (int i = 1; i < 8; i++)
	{
		XMVECTOR dist1 = XMVector3Dot(XMVector3Normalize(obb1->GetWorldMatAddr()->r[2]), vertices1[i]);
		XMVECTOR dist2 = XMVector3Dot(XMVector3Normalize(obb1->GetWorldMatAddr()->r[2]), vertices2[i]);
		if (min1 > dist1.m128_f32[0])
			min1 = dist1.m128_f32[0];
		if (min2 > dist2.m128_f32[0])
			min2 = dist2.m128_f32[0];
		if (max1 < dist1.m128_f32[0])
			max1 = dist1.m128_f32[0];
		if (max2 < dist2.m128_f32[0])
			max2 = dist2.m128_f32[0];
	}
	center1 = (max1 + min1) / 2.0f;
	center2 = (max2 + min2) / 2.0f;
	if ((max2 - center2) + (max1 - center1) < abs(center2 - center1))
		return false;

	// xaxis
	min1 = max1 = XMVector3Dot(XMVector3Normalize(obb2->GetWorldMatAddr()->r[0]), vertices1[0]).m128_f32[0];
	min2 = max2 = XMVector3Dot(XMVector3Normalize(obb2->GetWorldMatAddr()->r[0]), vertices2[0]).m128_f32[0];
	for (int i = 1; i < 8; i++)
	{
		XMVECTOR dist1 = XMVector3Dot(XMVector3Normalize(obb2->GetWorldMatAddr()->r[0]), vertices1[i]);
		XMVECTOR dist2 = XMVector3Dot(XMVector3Normalize(obb2->GetWorldMatAddr()->r[0]), vertices2[i]);
		if (min1 > dist1.m128_f32[0])
			min1 = dist1.m128_f32[0];
		if (min2 > dist2.m128_f32[0])
			min2 = dist2.m128_f32[0];
		if (max1 < dist1.m128_f32[0])
			max1 = dist1.m128_f32[0];
		if (max2 < dist2.m128_f32[0])
			max2 = dist2.m128_f32[0];
	}
	center1 = (max1 + min1) / 2.0f;
	center2 = (max2 + min2) / 2.0f;
	if ((max2 - center2) + (max1 - center1) < abs(center2 - center1))
		return false;

	// yaxis
	min1 = max1 = XMVector3Dot(XMVector3Normalize(obb2->GetWorldMatAddr()->r[1]), vertices1[0]).m128_f32[0];
	min2 = max2 = XMVector3Dot(XMVector3Normalize(obb2->GetWorldMatAddr()->r[1]), vertices2[0]).m128_f32[0];
	for (int i = 1; i < 8; i++)
	{
		XMVECTOR dist1 = XMVector3Dot(XMVector3Normalize(obb2->GetWorldMatAddr()->r[1]), vertices1[i]);
		XMVECTOR dist2 = XMVector3Dot(XMVector3Normalize(obb2->GetWorldMatAddr()->r[1]), vertices2[i]);
		if (min1 > dist1.m128_f32[0])
			min1 = dist1.m128_f32[0];
		if (min2 > dist2.m128_f32[0])
			min2 = dist2.m128_f32[0];
		if (max1 < dist1.m128_f32[0])
			max1 = dist1.m128_f32[0];
		if (max2 < dist2.m128_f32[0])
			max2 = dist2.m128_f32[0];
	}
	center1 = (max1 + min1) / 2.0f;
	center2 = (max2 + min2) / 2.0f;
	if ((max2 - center2) + (max1 - center1) < abs(center2 - center1))
		return false;

	// zaxis
	min1 = max1 = XMVector3Dot(XMVector3Normalize(obb2->GetWorldMatAddr()->r[2]), vertices1[0]).m128_f32[0];
	min2 = max2 = XMVector3Dot(XMVector3Normalize(obb2->GetWorldMatAddr()->r[2]), vertices2[0]).m128_f32[0];
	for (int i = 1; i < 8; i++)
	{
		XMVECTOR dist1 = XMVector3Dot(XMVector3Normalize(obb2->GetWorldMatAddr()->r[2]), vertices1[i]);
		XMVECTOR dist2 = XMVector3Dot(XMVector3Normalize(obb2->GetWorldMatAddr()->r[2]), vertices2[i]);
		if (min1 > dist1.m128_f32[0])
			min1 = dist1.m128_f32[0];
		if (min2 > dist2.m128_f32[0])
			min2 = dist2.m128_f32[0];
		if (max1 < dist1.m128_f32[0])
			max1 = dist1.m128_f32[0];
		if (max2 < dist2.m128_f32[0])
			max2 = dist2.m128_f32[0];
	}
	center1 = (max1 + min1) / 2.0f;
	center2 = (max2 + min2) / 2.0f;
	if ((max2 - center2) + (max1 - center1) < abs(center2 - center1))
		return false;

	return true;
}