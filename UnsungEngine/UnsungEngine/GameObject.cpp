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
	for (unsigned i = 0; i < components.size(); i++)
	{
		if (components[i])
			delete components[i];
	}
}

void GameObject::SetActive(bool _active)
{
	isActive = _active;
}

void GameObject::RemoveComponent(unsigned i)
{
	if (components[i])
		delete components[i];
	components.erase(i);
}

void GameObject::Init()
{
	if (isActive)
	{
		for (unsigned i = 0; i < components.size(); i++)
		{
			if (components[i]->GetType() == UEngine::ComponentType_SCRIPT)
				((ScriptComponent*)components[i])->Init();
		}
	}
}

void GameObject::Update()
{
	if (isActive)
	{
		if (renderComponent)
			renderComponent->Update();
		for (unsigned i = 0; i < components.size(); i++)
			components[i]->Update();
	}
}
