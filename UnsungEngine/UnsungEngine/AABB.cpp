#include "stdafx.h"
#include "AABB.h"


AABB::AABB() : CollisionBox()
{
}

AABB::AABB(DirectX::XMFLOAT2 x, DirectX::XMFLOAT2 y, DirectX::XMFLOAT2 z) : CollisionBox(nullptr, x, y, z)
{
}

AABB::~AABB()
{
}
