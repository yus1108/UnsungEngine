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
	// load model
	renderer.LoadObject("Assets/WOS_CommandCenter.bin", &gameObject);
}

void GameState::Update()
{
	// input

	// collision

	// update

	// render
	renderer.Update();
}