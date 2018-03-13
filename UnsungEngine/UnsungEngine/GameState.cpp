#include "stdafx.h"
#include "GameState.h"


GameState::GameState()
{
	gameObject = new GameObject();
	text = new GameObject();
	nullObject = new GameObject();
}

GameState::~GameState()
{
}

void GameState::Init()
{
	renderer.Init();
	objManager.Init();

	// load model
	renderer.LoadObject("Assets/WOS_CommandCenter.bin", gameObject);
	objManager.AddGameObject(gameObject);

	// load dump
	renderer.LoadObject("Assets/WOS_CommandCenter.bin", nullObject);
	objManager.AddGameObject(nullObject);

	// load text
	const WCHAR hello[] = L"Hello World!";
	unsigned textLength = ARRAYSIZE(hello) - 1;
	renderer.LoadGUI(hello, textLength, text);
	objManager.AddGameObject(text);

	objManager.RemoveGameObject(nullObject);
}

void GameState::Update()
{
	// input
	std::stringstream stringBuilder;
	stringBuilder << "Frame: " << utime.FramePerSecond() << std::endl;
	char pch[20];
	stringBuilder.getline(pch, 20);
	renderer.ChangeGUI(pch, text);

	// collision

	// update
	objManager.Update();

	// render
	renderer.Update(&objManager);
}