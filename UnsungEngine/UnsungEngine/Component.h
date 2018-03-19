#pragma once
class Component
{
private:
	bool isActive;
	UEngine::ComponentType type;
public:
	Component();
	virtual ~Component();

	bool GetActive() { return isActive; }
	void SetActive(bool _active) { isActive = _active; }
	UEngine::ComponentType GetType() { return type; }
	void SetType(UEngine::ComponentType _type) { type = _type; }

	virtual void Init(UEngine::ComponentType _type, bool _active) = 0;
};

