#include "stdafx.h"
#include "Component.h"


Component::Component()
{
}


Component::~Component()
{
}

void Component::Init(UEngine::ComponentType _type, bool _active, GameObject * _parent)
{
	SetType(_type);
	SetActive(_active);
	parentObject = _parent;
}
