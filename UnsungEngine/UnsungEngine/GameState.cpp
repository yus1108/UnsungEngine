#include "stdafx.h"
#include "GameState.h"


GameState::GameState()
{
	mainCamera = new GameObject();
	gameObject = new GameObject();
	gameObject2 = new GameObject();
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
	cameraComponent->Init(UEngine::ComponentType_CAMERA, true, mainCamera);
	cameraComponent->Init(&renderer, DirectX::XMFLOAT4(0, 0, 1, 1));
	mainCamera->AddComponent(cameraComponent);
	mainCamera->SetActive(false);
	objManager.AddGameObject(mainCamera);

	// load model
	renderer.LoadObject("Assets/WOS_CommandCenter.bin", gameObject);
	objManager.AddGameObject(gameObject);

	renderer.LoadObject("Assets/WOS_CommandCenter.bin", gameObject2);
	gameObject2->GetTransform()->SetMatrix(DirectX::XMMatrixMultiply(DirectX::XMMatrixIdentity(), DirectX::XMMatrixTranslation(0.5f, 0, 0)));
	objManager.AddGameObject(gameObject2);

	// load logo
	renderer.LoadGUI("Assets/TempLogo.png", logo);
	DirectX::XMMATRIX worldMat = DirectX::XMMatrixScaling(0.1f, 0.1f, 1);
	logo->GetTransform()->SetMatrix(worldMat);
	objManager.AddGameObject(logo);
	//objManager.RemoveGameObject(logo);

	// load text
	DirectX::XMMATRIX worldMat2 = DirectX::XMMatrixScaling(0.1f, 0.1f, 1);
	worldMat2.r[3] = DirectX::XMVectorSet(-0.92f, 0.95f, 0, 1);
	const WCHAR hello[] = L"Hello World!";
	unsigned textLength = ARRAYSIZE(hello) - 1;
	renderer.LoadGUI(hello, textLength, text);
	text->GetTransform()->SetMatrix(worldMat2);
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
		stringBuilder << "FPS: " << (unsigned)utime.FramePerSecond() << std::endl;
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