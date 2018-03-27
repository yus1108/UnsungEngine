#pragma once
#include "Renderer.h"
#include "ObjectManager.h"
#include "ThreadPool.h"

class ThreadPool;
extern ThreadPool threadPool;

class GameState
{
public:
	Renderer renderer;
	ObjectManager objManager;

	GameState();
	~GameState();

	void Init();
	void Update();
	void AddCamera(GameObject * cameraObject);
};

