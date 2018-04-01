#pragma once
#include "Renderer.h"
#include "CollisionManager.h"
#include "ObjectManager.h"
#include "ThreadPool.h"

class ThreadPool;
extern ThreadPool threadPool;

class GameState
{
private:
	bool firstRun = false;
public:
	Renderer * renderer;
	CollisionManager * collision;
	ObjectManager * objManager;

	GameState();
	~GameState();

	void Init();
	void Update();
	void AddCamera(GameObject * cameraObject);
	void Restart(const char * sceneName);
};

