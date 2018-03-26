#pragma once
#include "Component.h"
class ScriptComponent :
	public Component
{
private:
	float updateCounter;
public:
	ScriptComponent();
	virtual ~ScriptComponent();

	virtual void Init(UEngine::ComponentType _type, bool _active, GameObject * _parent);
	void Update();

	virtual void Init() {};
	virtual void FrameUpdate() {};
	virtual void FixedUpdate() {};
	virtual void LateUpdate() {};
};

