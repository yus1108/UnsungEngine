#pragma once
class Transform
{
	DirectX::XMMATRIX matrix;
public:
	Transform();
	~Transform();

	DirectX::XMMATRIX GetMatrix() { return matrix; }
	void SetMatrix(DirectX::XMMATRIX _matrix) { matrix = _matrix; }

	DirectX::XMFLOAT3 GetPosition3();
	DirectX::XMFLOAT4 GetPosition4();
	DirectX::XMVECTOR GetPositionVec() { return matrix.r[3]; }
	void SetPosition(DirectX::XMVECTOR _position);
	void SetPosition(DirectX::XMFLOAT3 _position);

	DirectX::XMFLOAT3 GetScale();
};

