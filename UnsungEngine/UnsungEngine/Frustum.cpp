#include "stdafx.h"
#include "Frustum.h"


Frustum::Frustum()
{
}

Frustum::Frustum(DirectX::XMMATRIX * _worldMat, DirectX::XMFLOAT2 xNear, DirectX::XMFLOAT2 yNear, 
	float zNear, DirectX::XMFLOAT2 xFar, DirectX::XMFLOAT2 yFar, float zFar) : CollisionBox(_worldMat, xNear, yNear, DirectX::XMFLOAT2(zNear, zFar), UEngine::Collision_Frustum)
{
	xaxisFar = xFar;
	yaxisFar = yFar;

	vertices[0] = DirectX::XMVectorSet(xNear.x, yNear.x, zNear, 1);
	vertices[1] = DirectX::XMVectorSet(xNear.x, yNear.y, zNear, 1);
	vertices[2] = DirectX::XMVectorSet(xNear.y, yNear.x, zNear, 1);
	vertices[3] = DirectX::XMVectorSet(xNear.y, yNear.y, zNear, 1);
	vertices[4] = DirectX::XMVectorSet(xFar.x, yFar.x, zFar, 1);
	vertices[5] = DirectX::XMVectorSet(xFar.x, yFar.y, zFar, 1);
	vertices[6] = DirectX::XMVectorSet(xFar.y, yFar.x, zFar, 1);
	vertices[7] = DirectX::XMVectorSet(xFar.y, yFar.y, zFar, 1);
}


Frustum::~Frustum()
{
}
