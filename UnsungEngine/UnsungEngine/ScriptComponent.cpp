#include "stdafx.h"
#include "ScriptComponent.h"


ScriptComponent::ScriptComponent()
{
}


ScriptComponent::~ScriptComponent()
{
}

void ScriptComponent::Init(UEngine::ComponentType _type, bool _active, GameObject * _parent)
{
	Component::Init(_type, _active, _parent);
	updateCounter = 0;
}

void ScriptComponent::Update()
{
	// per frame update
	FrameUpdate();

	// fixed update
	updateCounter += (float)utime.DeltaTime();
	if (updateCounter >= 0.2f)
	{
		FixedUpdate();
		updateCounter = 0;
	}

	LateUpdate();
}
