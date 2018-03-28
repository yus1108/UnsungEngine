#pragma once
#include "CollisionBox.h"
class Frustum :
	public CollisionBox
{
private:
	DirectX::XMFLOAT2 xaxisFar;
	DirectX::XMFLOAT2 yaxisFar;
	DirectX::XMVECTOR vertices[8];
public:
	Frustum();
	Frustum(DirectX::XMMATRIX * _worldMat, DirectX::XMFLOAT2 xNear, DirectX::XMFLOAT2 yNear, float zNear,
		DirectX::XMFLOAT2 xFar, DirectX::XMFLOAT2 yFar, float zFar);
	virtual ~Frustum();

	DirectX::XMVECTOR GetVertex(unsigned i) { return vertices[i]; }
};

