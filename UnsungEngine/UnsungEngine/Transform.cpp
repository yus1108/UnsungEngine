#include "stdafx.h"
#include "Transform.h"


Transform::Transform()
{
	matrix = DirectX::XMMatrixIdentity();
}


Transform::~Transform()
{
}

DirectX::XMFLOAT3 Transform::GetScale()
{
	float zAxis = DirectX::XMVector3Length(matrix.r[2]).m128_f32[0];
	float xAxis = DirectX::XMVector3Length(matrix.r[0]).m128_f32[0];
	float yAxis = DirectX::XMVector3Length(matrix.r[1]).m128_f32[0];
	return DirectX::XMFLOAT3(xAxis, yAxis, zAxis);
}
