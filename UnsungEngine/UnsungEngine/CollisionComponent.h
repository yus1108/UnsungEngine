#pragma once
#include "Component.h"

class CollisionComponent :
	public Component
{
private:
	CollisionBox * collisionBox;
public:
	CollisionComponent();
	virtual ~CollisionComponent();

	CollisionBox * GetCollisionBox() { return collisionBox; }
	void SetCollisionBox(CollisionBox * _collisionBox) { collisionBox = _collisionBox; }

	void Init(UEngine::ComponentType _type, bool _active, GameObject * _parent);
	void Update();
};

