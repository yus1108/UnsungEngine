#pragma once
#include "Component.h"

class CollisionComponent :
	public Component
{
public:
	CollisionComponent();
	virtual ~CollisionComponent();

	void Init(UEngine::ComponentType _type, bool _active, GameObject * _parent);
	void Update();
};

