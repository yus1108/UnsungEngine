#pragma once
class Transform
{
	DirectX::XMMATRIX matrix;
	DirectX::XMVECTOR prevPos;
public:
	Transform();
	~Transform();

	DirectX::XMMATRIX GetMatrix() { return matrix; }
	DirectX::XMMATRIX * GetMatrixAddr() { return &matrix; }
	void SetMatrix(DirectX::XMMATRIX _matrix);

	DirectX::XMFLOAT3 GetPosition3();
	DirectX::XMFLOAT4 GetPosition4();
	DirectX::XMVECTOR GetPositionVec() { return matrix.r[3]; }
	void SetPosition(DirectX::XMVECTOR _position);
	void SetPosition(DirectX::XMFLOAT3 _position);
	DirectX::XMVECTOR GetPrevPos() { return prevPos; }
	void SetPrevPos() { prevPos = matrix.r[3]; }

	DirectX::XMFLOAT3 GetScale();
};

