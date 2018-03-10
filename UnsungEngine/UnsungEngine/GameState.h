#pragma once
#include "Renderer.h"

class GameState
{
private:
	Renderer renderer;
	GameObject gameObject;
public:
	GameState();
	~GameState();

	void Init();
	void Update();
};

