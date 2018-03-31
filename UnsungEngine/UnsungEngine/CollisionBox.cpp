#include "stdafx.h"
#include "CollisionBox.h"


CollisionBox::CollisionBox(DirectX::XMMATRIX * _worldMat, DirectX::XMFLOAT2 x, DirectX::XMFLOAT2 y, DirectX::XMFLOAT2 z, UEngine::CollisionType _collisionType)
{
	worldMat = _worldMat;
	xaxis = x;
	yaxis = y;
	zaxis = z;
	collisionType = _collisionType;
}

CollisionBox::~CollisionBox()
{
}
