#include "stdafx.h"
#include "OOBB.h"


OOBB::OOBB() : CollisionBox()
{
}

OOBB::OOBB(DirectX::XMMATRIX * _worldMat, DirectX::XMFLOAT2 x, DirectX::XMFLOAT2 y, DirectX::XMFLOAT2 z) : CollisionBox(_worldMat, x, y, z, UEngine::Collision_OOBB)
{
	vertices[0] = DirectX::XMVectorSet(xaxis.x, yaxis.x, zaxis.x, 1);
	vertices[1] = DirectX::XMVectorSet(xaxis.x, yaxis.x, zaxis.y, 1);
	vertices[2] = DirectX::XMVectorSet(xaxis.x, yaxis.y, zaxis.x, 1);
	vertices[3] = DirectX::XMVectorSet(xaxis.x, yaxis.y, zaxis.y, 1);
	vertices[4] = DirectX::XMVectorSet(xaxis.y, yaxis.x, zaxis.x, 1);
	vertices[5] = DirectX::XMVectorSet(xaxis.y, yaxis.x, zaxis.y, 1);
	vertices[6] = DirectX::XMVectorSet(xaxis.y, yaxis.y, zaxis.x, 1);
	vertices[7] = DirectX::XMVectorSet(xaxis.y, yaxis.y, zaxis.y, 1);
}

OOBB::~OOBB()
{
}
