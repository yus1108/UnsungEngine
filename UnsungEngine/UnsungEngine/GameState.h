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

