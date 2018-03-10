#include "stdafx.h"
#include "GameObject.h"


GameObject::GameObject()
{
	renderComponent = nullptr;
}


GameObject::~GameObject()
{
	if (renderComponent)
		delete renderComponent;
}
