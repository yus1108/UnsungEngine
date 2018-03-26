#pragma once
#include "Component.h"
#include "Transform.h"
#include "RenderComponent.h"

class GameObject
{
private:
	Transform transform;
	RenderComponent * renderComponent;
	UVector<Component*> components;
	unsigned referenceNum;
	bool isActive;
public:
	GameObject();
	~GameObject();

	unsigned GetReferenceNum() { return referenceNum; }
	void SetReferenceNum(unsigned _ref) { referenceNum = _ref; }
	bool GetActive() { return isActive; }
	void SetActive(bool _active);

	Transform * GetTransform() { return &transform; }
	void SetTransform(Transform * _transform) { transform = *_transform; }
	RenderComponent * GetRenderComponent() { return renderComponent; }
	void SetRenderComponent(RenderComponent * _renderComponent) { renderComponent = _renderComponent; }

	void ClearComponents() { components.clear(); }
	unsigned GetSizeComponents() { return components.size(); }
	void AddComponent(Component * component) { components.push_back(component); }
	Component * GetComponent(unsigned i) { return components[i]; }
	void RemoveComponent(unsigned i);

	virtual void Init();
	virtual void Update();
};

