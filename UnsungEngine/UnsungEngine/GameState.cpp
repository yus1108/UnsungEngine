#include "stdafx.h"
#include "GameState.h"


GameState::GameState()
{
	gameObject = new GameObject();
	text = new GameObject();
	logo = new GameObject();
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
	renderer.LoadGUI("Assets/TempLogo.png", logo);
	objManager.AddGameObject(logo);
	objManager.RemoveGameObject(logo);

	// load text
	const WCHAR hello[] = L"Hello World!";
	unsigned textLength = ARRAYSIZE(hello) - 1;
	renderer.LoadGUI(hello, textLength, text);
	DirectX::XMMATRIX worldMat2 = DirectX::XMMatrixScaling(0.1f, 0.1f, 1);
	text->GetTransform()->SetMatrix(worldMat2);
	objManager.AddGameObject(text);
}

void GameState::Update()
{
	// input
	std::stringstream stringBuilder;
	stringBuilder << "Frame: " << utime.FramePerSecond() << std::endl;
	char pch[20];
	stringBuilder.getline(pch, 20);
	renderer.ChangeGUI(pch, text);

	DirectX::XMMATRIX worldMat = DirectX::XMMatrixMultiply(gameObject->GetTransform()->GetMatrix(), DirectX::XMMatrixRotationY((float)utime.DeltaTime() / 10.0f));
	gameObject->GetTransform()->SetMatrix(worldMat);

	// collision

	// update
	objManager.Update();

	// render
	renderer.Update(&objManager);
}