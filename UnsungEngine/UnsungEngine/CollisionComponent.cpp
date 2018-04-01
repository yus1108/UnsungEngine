#include "stdafx.h"
#include "CollisionComponent.h"


CollisionComponent::CollisionComponent()
{
}


CollisionComponent::~CollisionComponent()
{
}

void CollisionComponent::Init(UEngine::ComponentType _type, bool _active, GameObject * _parent)
{
	Component::Init(_type, _active, _parent);
	isStatic = false;
	isTrigger = false;
}

void CollisionComponent::Update()
{
}
