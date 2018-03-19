#include "stdafx.h"
#include "GameState.h"


GameState::GameState()
{
	camera = new GameObject();
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

	CameraComponent * cameraComponent = new CameraComponent();
	cameraComponent->Init(UEngine::ComponentType_CAMERA, true, camera);
	cameraComponent->Init(&renderer);
	camera->AddComponent(cameraComponent);
	camera->SetActive(false);
	objManager.AddGameObject(camera);

	// load model
	renderer.LoadObject("Assets/WOS_CommandCenter.bin", gameObject);
	objManager.AddGameObject(gameObject);

	// load logo
	renderer.LoadGUI("Assets/TempLogo.png", logo);
	DirectX::XMMATRIX worldMat = DirectX::XMMatrixScaling(0.1f, 0.1f, 1);
	logo->GetTransform()->SetMatrix(worldMat);
	objManager.AddGameObject(logo);
	objManager.RemoveGameObject(logo);

	// load text
	const WCHAR hello[] = L"Hello World!";
	unsigned textLength = ARRAYSIZE(hello) - 1;
	renderer.LoadGUI(hello, textLength, text);
	text->GetTransform()->SetMatrix(worldMat);
	//text->SetActive(false);
	objManager.AddGameObject(text);

	//renderer.Resize(true, 1920, 1024);
	//renderer.Resize(false, 1280, 768);
}

void GameState::Update()
{
	// input
	if (input.GetKeyPress('U'))
	{
		std::cout << "U is pressed" << std::endl;
	}
	if (input.GetKey('I'))
	{
		std::cout << "I is pressed" << std::endl;
	}

	if (input.GetMousePress(UEngine::MouseInputType_LEFT))
	{
		DirectX::XMFLOAT2 ndcPos = input.GetMousePos();
		std::cout << "mouse pos: " << ndcPos.x << ", " << ndcPos.y << std::endl;
	}

	if (text->GetActive())
	{
		std::stringstream stringBuilder;
		stringBuilder << "Frame: " << utime.FramePerSecond() << std::endl;
		char pch[20];
		stringBuilder.getline(pch, 20);
		renderer.ChangeGUI(pch, text);
	}
	
	if (gameObject->GetActive())
	{
		DirectX::XMMATRIX worldMat = DirectX::XMMatrixMultiply(gameObject->GetTransform()->GetMatrix(), DirectX::XMMatrixRotationY((float)utime.DeltaTime() / 10.0f));
		gameObject->GetTransform()->SetMatrix(worldMat);
	}

	// collision

	// update
	objManager.Update();

	// render
	renderer.Update(&objManager);
}