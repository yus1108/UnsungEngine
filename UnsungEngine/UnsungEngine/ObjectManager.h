#pragma once
#include <unordered_map>
#include "GameObject.h"
#include "CameraComponent.h"

class ObjectManager
{
	unsigned obj_reference_num;
	std::unordered_map<unsigned, GameObject*> gameObjects;
public:
	ObjectManager();
	~ObjectManager();

	void Init();
	void Update();
	void Render(CameraComponent * m_pCamera, Renderer * render);
	void Clear();

	GameObject * GetGameObject(unsigned i);
	void AddGameObject(GameObject * _obj);
	void RemoveGameObject(GameObject * _obj);
};

