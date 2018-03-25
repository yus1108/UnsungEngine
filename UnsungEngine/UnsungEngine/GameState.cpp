#include "stdafx.h"
#include "GameState.h"


GameState::GameState()
{
	mainCamera = new GameObject();
	txt_frameRate = new GameObject();
	mainCamera2 = new GameObject();
	gameObject = new GameObject();
	logo = new GameObject();
	particle = new GameObject();
	numParticles = new GameObject();
}

GameState::~GameState()
{
}

void GameState::Init()
{
	renderer.Init();
	objManager.Init();

	// load main camera
	CameraComponent * cameraComponent = new CameraComponent();
	cameraComponent->Init(UEngine::ComponentType_CAMERA, true, mainCamera);
	cameraComponent->Init(&renderer, DirectX::XMFLOAT4(0, 0, 1, 1));
	mainCamera->AddComponent(cameraComponent);
	mainCamera->SetActive(true);
	objManager.AddGameObject(mainCamera);

	// load frame rate
	DirectX::XMMATRIX worldMat2 = DirectX::XMMatrixScaling(0.1f, 0.1f, 1);
	worldMat2.r[3] = DirectX::XMVectorSet(-0.92f, 0.95f, 0, 1);
	const WCHAR hello[] = L"Welcome to Unsung Engine!";
	unsigned textLength = ARRAYSIZE(hello) - 1;
	UEngine::TextFormat textFormat;
	textFormat.textColor = D2D1::ColorF::White;
	textFormat.dpiX = 100;
	textFormat.dpiY = 400;
	textFormat.width = 400;
	textFormat.height = 200;
	renderer.LoadGUI(hello, textLength, txt_frameRate, 0, textFormat);
	txt_frameRate->GetTransform()->SetMatrix(worldMat2);
	//text->SetActive(false);
	objManager.AddGameObject(txt_frameRate);

	// load secondary camera
	CameraComponent * cameraComponent2 = new CameraComponent();
	cameraComponent2->Init(UEngine::ComponentType_CAMERA, true, mainCamera2);
	cameraComponent2->Init(&renderer, DirectX::XMFLOAT4(0.78f, 0.05f, 0.98f, 0.25f));
	DirectX::XMMATRIX camera2Pos = DirectX::XMMatrixRotationY(UMath::Convert_DegreeToRad(-90.0f));
	camera2Pos.r[3] = DirectX::XMVectorSet(20, 5, 0, 1);
	cameraComponent2->SetOriginalView(camera2Pos);
	mainCamera2->AddComponent(cameraComponent2);
	//mainCamera2->SetActive(false);
	objManager.AddGameObject(mainCamera2);

	// load particle
	renderer.LoadParticle("Assets/acid-front.jpg", particle);
	objManager.AddGameObject(particle);

	// load object 2
	renderer.LoadObject("Assets/WOS_CommandCenter.bin", gameObject);
	gameObject->GetTransform()->SetMatrix(DirectX::XMMatrixMultiply(DirectX::XMMatrixIdentity(), DirectX::XMMatrixTranslation(0.5f, 0, 0)));
	//gameObject->SetActive(false);
	objManager.AddGameObject(gameObject);

	// load logo
	renderer.LoadGUI("Assets/TempLogo.pnj", logo);
	DirectX::XMMATRIX worldMat = DirectX::XMMatrixScaling(0.1f, 0.1f, 1);
	logo->GetTransform()->SetMatrix(worldMat);
	//logo->SetActive(false);
	objManager.AddGameObject(logo);
	//objManager.RemoveGameObject(logo);

	// load logo2
	textFormat.dpiX = 80;
	renderer.LoadGUI(hello, textLength, numParticles, 0, textFormat);
	worldMat2.r[3] = DirectX::XMVectorSet(-0.905f, 0.87f, 0, 1);
	numParticles->GetTransform()->SetMatrix(worldMat2);
	//text->SetActive(false);
	objManager.AddGameObject(numParticles);

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

	if (txt_frameRate->GetActive())
	{
		std::stringstream stringBuilder;
		stringBuilder << "FPS: " << (unsigned)utime.FramePerSecond() << std::endl;
		char pch[20];
		stringBuilder.getline(pch, 20);
		renderer.ChangeGUI(pch, txt_frameRate);
	}

	if (numParticles->GetActive())
	{
		std::stringstream stringBuilder;
		stringBuilder << "Particles: " << (unsigned)((Render_Particle*)particle->GetRenderComponent())->GetNumParticles() << std::endl;
		char pch[20];
		stringBuilder.getline(pch, 20);
		renderer.ChangeGUI(pch, numParticles);
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