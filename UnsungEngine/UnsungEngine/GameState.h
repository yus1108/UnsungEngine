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
private:
	GameObject * mainCamera;
	GameObject * txt_frameRate;
	GameObject * mainCamera2;
	GameObject * gameObject;
	GameObject * logo;
	GameObject * particle;
	GameObject * numParticles;
public:
	GameState();
	~GameState();

	void Init();
	void Update();
	void AddCamera(GameObject * cameraObject);
};

