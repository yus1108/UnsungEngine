#include "stdafx.h"
#include "Transform.h"


Transform::Transform()
{
	matrix = DirectX::XMMatrixIdentity();
}


Transform::~Transform()
{
}

void Transform::SetMatrix(DirectX::XMMATRIX _matrix)
{
	matrix = _matrix;
}

DirectX::XMFLOAT3 Transform::GetPosition3()
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMStoreFloat3(&pos, matrix.r[3]);
	return pos;
}

DirectX::XMFLOAT4 Transform::GetPosition4()
{
	DirectX::XMFLOAT4 pos; 
	DirectX::XMStoreFloat4(&pos, matrix.r[3]); 
	return pos;
}

void Transform::SetPosition(DirectX::XMVECTOR _position)
{
	matrix.r[3] = _position;
	matrix.r[3].m128_f32[3] = 1;
}

void Transform::SetPosition(DirectX::XMFLOAT3 _position)
{
	matrix.r[3] = DirectX::XMLoadFloat3(&_position);
	matrix.r[3].m128_f32[3] = 1;
}

DirectX::XMFLOAT3 Transform::GetScale()
{
	float zAxis = DirectX::XMVector3Length(matrix.r[2]).m128_f32[0];
	float xAxis = DirectX::XMVector3Length(matrix.r[0]).m128_f32[0];
	float yAxis = DirectX::XMVector3Length(matrix.r[1]).m128_f32[0];
	return DirectX::XMFLOAT3(xAxis, yAxis, zAxis);
}
