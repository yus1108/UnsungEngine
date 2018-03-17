#include "stdafx.h"
#include "GameObject.h"


GameObject::GameObject()
{
	renderComponent = nullptr;
	isActive = true;
}


GameObject::~GameObject()
{
	if (renderComponent)
		delete renderComponent;
}

void GameObject::SetActive(bool _active)
{
	isActive = _active;
}

void GameObject::Update()
{
	if (isActive)
	{

	}
}
