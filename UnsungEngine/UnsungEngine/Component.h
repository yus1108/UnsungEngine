#pragma once
#include "GameObject.h"
#include "SystemDefinition.h"

class GameObject;
class Component
{
protected:
	bool isActive;
	UEngine::ComponentType type;
	GameObject * parentObject;
public:
	Component();
	virtual ~Component();

	bool GetActive() { return isActive; }
	void SetActive(bool _active) { isActive = _active; }
	UEngine::ComponentType GetType() { return type; }
	void SetType(UEngine::ComponentType _type) { type = _type; }
	GameObject * GetParent() { return parentObject; }

	virtual void Init(UEngine::ComponentType _type, bool _active, GameObject * _parent) = 0;
	virtual void Update() = 0;
};

