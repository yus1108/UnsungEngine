#pragma once
#include "CollisionBox.h"
class AABB :
	public CollisionBox
{
	friend class CollisionManager;
public:
	AABB();
	AABB(DirectX::XMFLOAT2 x, DirectX::XMFLOAT2 y, DirectX::XMFLOAT2 z);
	virtual ~AABB();
};

