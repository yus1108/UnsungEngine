#pragma once
#include "Renderer.h"
#include "ObjectManager.h"

class GameState
{
private:
	Renderer renderer;
	ObjectManager objManager;
	GameObject gameObject;
	GameObject text;
public:
	GameState();
	~GameState();

	void Init();
	void Update();
};

