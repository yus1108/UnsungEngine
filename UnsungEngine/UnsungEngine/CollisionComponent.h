#pragma once
#include "Component.h"

class CollisionComponent :
	public Component
{
private:
	CollisionBox * collisionBox;
	bool isStatic;
	bool isTrigger;
public:
	CollisionComponent();
	virtual ~CollisionComponent();

	CollisionBox * GetCollisionBox() { return collisionBox; }
	void SetCollisionBox(CollisionBox * _collisionBox) { collisionBox = _collisionBox; }
	bool GetStatic() { return isStatic; }
	void SetStatic(bool _isStatic) { isStatic = _isStatic; }
	bool GetTrigger() { return isTrigger; }
	void SetTrigger(bool _isTrigger) { isTrigger = _isTrigger; }

	void Init(UEngine::ComponentType _type, bool _active, GameObject * _parent);
	void Update();
};

