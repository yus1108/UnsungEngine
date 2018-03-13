#pragma once
class Transform
{
	DirectX::XMMATRIX matrix;
public:
	Transform();
	~Transform();

	DirectX::XMMATRIX GetMatrix() { return matrix; }
	void SetMatrix(DirectX::XMMATRIX _matrix) { matrix = _matrix; }

	DirectX::XMFLOAT3 GetScale();
};

