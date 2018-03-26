#pragma once
class OOBB : public CollisionBox
{
private:
	DirectX::XMVECTOR vertices[8];
public:
	OOBB();
	OOBB(DirectX::XMMATRIX * _worldMat, DirectX::XMFLOAT2 x, DirectX::XMFLOAT2 y, DirectX::XMFLOAT2 z);
	virtual ~OOBB();

	DirectX::XMVECTOR GetVertex(unsigned i) { return vertices[i]; }
};

