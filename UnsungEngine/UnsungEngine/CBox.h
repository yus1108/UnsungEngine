#pragma once
class CollisionBox
{
protected:
	DirectX::XMFLOAT2 xaxis;
	DirectX::XMFLOAT2 yaxis;
	DirectX::XMFLOAT2 zaxis;
	DirectX::XMMATRIX * worldMat;
public:
	CollisionBox() {};
	CollisionBox(DirectX::XMMATRIX * _worldMat, DirectX::XMFLOAT2 x, DirectX::XMFLOAT2 y, DirectX::XMFLOAT2 z);
	virtual ~CollisionBox();

	DirectX::XMFLOAT2 GetXAxis() { return xaxis; }
	void SetXAxis(DirectX::XMFLOAT2 x) { xaxis = x; }
	DirectX::XMFLOAT2 GetYAxis() { return yaxis; }
	void SetYAxis(DirectX::XMFLOAT2 y) { yaxis = y; }
	DirectX::XMFLOAT2 GetZAxis() { return zaxis; }
	void SetZAxis(DirectX::XMFLOAT2 z) { zaxis = z; }
	DirectX::XMMATRIX GetWorldMat() { return *worldMat; }
	DirectX::XMMATRIX * GetWorldMatAddr() { return worldMat; }
	void SetWorldMat(DirectX::XMMATRIX * _worldMat) { worldMat = _worldMat; }
};

