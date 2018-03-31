#pragma once
// TODO: Add new collision bounding type
namespace UEngine {
	enum CollisionType {
		Collision_AABB,
		Collision_OOBB,
		Collision_Frustum,
		Collision_COUNT
	};
}

class CollisionBox
{
protected:
	DirectX::XMFLOAT2 xaxis;
	DirectX::XMFLOAT2 yaxis;
	DirectX::XMFLOAT2 zaxis;
	DirectX::XMMATRIX * worldMat;
	UEngine::CollisionType collisionType;
public:
	CollisionBox() {};
	CollisionBox(DirectX::XMMATRIX * _worldMat, DirectX::XMFLOAT2 x, DirectX::XMFLOAT2 y, DirectX::XMFLOAT2 z, UEngine::CollisionType _collisionType);
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
	UEngine::CollisionType GetCollisionType() { return collisionType; }
	void SetCollisionType(UEngine::CollisionType _type) { collisionType = _type; }
};

