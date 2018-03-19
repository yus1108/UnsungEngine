#include "stdafx.h"
#include "Component.h"


Component::Component()
{
}


Component::~Component()
{
}

void Component::Init(UEngine::ComponentType _type, bool _active)
{
	SetType(_type);
	SetActive(_active);
}
