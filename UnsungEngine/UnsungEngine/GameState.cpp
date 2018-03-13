#include "stdafx.h"
#include "GameState.h"


GameState::GameState()
{
}

GameState::~GameState()
{
}

void GameState::Init()
{
	renderer.Init();
	objManager.Init();

	// load model
	renderer.LoadObject("Assets/WOS_CommandCenter.bin", &gameObject);
	objManager.AddObj(&gameObject);

	// load text
	const WCHAR hello[] = L"Hello World!";
	unsigned textLength = ARRAYSIZE(hello) - 1;
	renderer.LoadGUI(hello, textLength, &text);
	objManager.AddObj(&text);
}

void GameState::Update()
{
	// input
	std::stringstream stringBuilder;
	stringBuilder << "Frame: " << utime.FramePerSecond() << std::endl;
	char pch[20];
	stringBuilder.getline(pch, 20);
	renderer.ChangeGUI(pch, &text);

	// collision

	// update
	objManager.Update();

	// render
	renderer.Update(&objManager);
}