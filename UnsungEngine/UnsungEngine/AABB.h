#pragma once
#include "CollisionBox.h"
class AABB :
	public CollisionBox
{
public:
	AABB();
	AABB(DirectX::XMFLOAT2 x, DirectX::XMFLOAT2 y, DirectX::XMFLOAT2 z);
	virtual ~AABB();
};

