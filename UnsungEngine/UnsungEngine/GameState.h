#pragma once
#include "Renderer.h"
#include "ObjectManager.h"

extern ThreadPool threadPool;

class GameState
{
private:
	Renderer renderer;
	ObjectManager objManager;
	GameObject * mainCamera;
	GameObject * mainCamera2;
	GameObject * gameObject;
	GameObject * gameObject2;
	GameObject * text;
	GameObject * logo;
public:
	GameState();
	~GameState();

	void Init();
	void Update();
};

