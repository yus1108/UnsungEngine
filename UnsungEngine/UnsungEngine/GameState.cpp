#include "stdafx.h"
#include "GameState.h"

// include script created
#include "BuildingScript.h"
#include "FrameTextScript.h"
#include "NumParticleScript.h"
#include "CameraScript.h"

GameState::GameState()
{
	renderer = new Renderer();
	objManager = new ObjectManager();
	collision = new CollisionManager();
}

GameState::~GameState()
{
	if (collision)
		delete collision;
	if (objManager)
		delete objManager;
	if (renderer)
	{
		while (!renderer->loadingDone)
		{

		}
		delete renderer;
	}
}

void GameState::Init()
{
	firstRun = true;
	renderer->Init();
	AABB map = AABB(XBOUNDARY, YBOUNDARY, ZBOUNDARY);
	collision->Init(map, COLLISION_LEVEL);

	// load main camera 0
	GameObject * mainCamera = new GameObject();
	DirectX::XMMATRIX cameraPos = DirectX::XMMatrixIdentity();
	cameraPos.r[3] = DirectX::XMVectorSet(0, 0, -10, 1);
	mainCamera->GetTransform()->SetMatrix(cameraPos);
	AddCamera(mainCamera);
	Component * cameraScript = new CameraScript();
	cameraScript->Init(UEngine::ComponentType_SCRIPT, true, mainCamera);
	mainCamera->AddComponent(cameraScript);
	objManager->AddGameObject(mainCamera);

	// load frame rate 1
	GameObject * txt_frameRate = new GameObject();
	const WCHAR hello[] = L"Text";
	unsigned textLength = ARRAYSIZE(hello) - 1;
	UEngine::TextFormat textFormat;
	textFormat.textColor = D2D1::ColorF::White;
	textFormat.dpiX = 100;
	textFormat.dpiY = 400;
	textFormat.width = 400;
	textFormat.height = 200;
	renderer->LoadGUI(hello, textLength, txt_frameRate, 0, textFormat);
	DirectX::XMMATRIX textWorldmat = DirectX::XMMatrixScaling(0.1f, 0.1f, 1);
	textWorldmat.r[3] = DirectX::XMVectorSet(-0.92f, 0.94f, 0, 1);
	txt_frameRate->GetTransform()->SetMatrix(textWorldmat);
	Component * textScript = new FrameTextScript();
	textScript->Init(UEngine::ComponentType_SCRIPT, true, txt_frameRate);
	txt_frameRate->AddComponent(textScript);
	//txt_frameRate->SetActive(false);
	objManager->AddGameObject(txt_frameRate);

	// load secondary camera 2
	GameObject * mainCamera2 = new GameObject();
	DirectX::XMMATRIX camera2Pos = DirectX::XMMatrixRotationY(UMath::Convert_DegreeToRad(-90.0f));
	camera2Pos.r[3] = DirectX::XMVectorSet(20, 0, 0, 1);
	mainCamera2->GetTransform()->SetMatrix(camera2Pos);
	CameraComponent * cameraComponent2 = new CameraComponent();
	cameraComponent2->Init(UEngine::ComponentType_CAMERA, true, mainCamera2);
	cameraComponent2->Init(renderer, DirectX::XMFLOAT4(0.78f, 0.05f, 0.98f, 0.25f));
	mainCamera2->AddComponent(cameraComponent2);
	//mainCamera2->SetActive(false);
	objManager->AddGameObject(mainCamera2);

	// load particle 3
	GameObject * particle = new GameObject();
	renderer->LoadParticle("Assets/particle.png", particle);
	DirectX::XMMATRIX particleWorldmat = DirectX::XMMatrixScaling(0.1f, 0.1f, 0.1f);
	particleWorldmat.r[3] = DirectX::XMVectorSet(5, 5, 5, 1);
	particle->GetTransform()->SetMatrix(particleWorldmat);
	//particle->SetActive(false);
	objManager->AddGameObject(particle);

	// load object 4
	GameObject * gameObject = new GameObject();
	renderer->LoadObject("Assets/GreenCube.bin", gameObject);
	gameObject->GetTransform()->SetMatrix(DirectX::XMMatrixMultiply(DirectX::XMMatrixIdentity(), DirectX::XMMatrixTranslation(0, 0, 0)));
	Component * buildScript = new BuildingScript();
	buildScript->Init(UEngine::ComponentType_SCRIPT, true, gameObject);
	gameObject->AddComponent(buildScript);
	CollisionComponent * collisionComp = new CollisionComponent();
	collisionComp->Init(UEngine::ComponentType_COLLISION, true, gameObject);
	collisionComp->SetCollisionBox(gameObject->GetRenderComponent()->GetCollisionBox());
	gameObject->AddComponent(collisionComp);
	//gameObject->SetActive(false);
	objManager->AddGameObject(gameObject);

	// load logo 5
	GameObject * logo = new GameObject();
	renderer->LoadGUI("Assets/TempLogo.png", logo, 0);
	DirectX::XMMATRIX worldMat = DirectX::XMMatrixScaling(0.1f, 0.1f, 0.1f);
	worldMat.r[3] = DirectX::XMVectorSet(0, 0, 0, 1);
	logo->GetTransform()->SetMatrix(worldMat);
	objManager->AddGameObject(logo);
	objManager->RemoveGameObject(logo);

	// load numParticles 5/6
	GameObject * numParticles = new GameObject();
	textFormat.dpiX = 80;
	renderer->LoadGUI(hello, textLength, numParticles, 0, textFormat);
	DirectX::XMMATRIX prticleTextWorldmat = DirectX::XMMatrixScaling(0.1f, 0.1f, 1);
	prticleTextWorldmat.r[3] = DirectX::XMVectorSet(-0.905f, 0.87f, 0, 1);
	numParticles->GetTransform()->SetMatrix(prticleTextWorldmat);
	Component * numParticleScript = new NumParticleScript();
	numParticleScript->Init(UEngine::ComponentType_SCRIPT, true, numParticles);
	numParticles->AddComponent(numParticleScript);
	//numParticles->SetActive(false);
	objManager->AddGameObject(numParticles);

	// load object 4
	GameObject * gameObject2 = new GameObject();
	renderer->LoadObject("Assets/GreenCube.bin", gameObject2);
	gameObject2->GetTransform()->SetMatrix(DirectX::XMMatrixMultiply(DirectX::XMMatrixIdentity(), DirectX::XMMatrixScaling(10, 10, 10)));
	gameObject2->GetTransform()->SetMatrix(DirectX::XMMatrixMultiply(gameObject2->GetTransform()->GetMatrix(), DirectX::XMMatrixRotationZ(UMath::Convert_DegreeToRad(20.0f))));
	gameObject2->GetTransform()->SetMatrix(DirectX::XMMatrixMultiply(gameObject2->GetTransform()->GetMatrix(), DirectX::XMMatrixTranslation(20, 5, 0)));
	CollisionComponent * collisionComp2 = new CollisionComponent();
	collisionComp2->Init(UEngine::ComponentType_COLLISION, true, gameObject2);
	collisionComp2->SetCollisionBox(gameObject2->GetRenderComponent()->GetCollisionBox());
	collisionComp2->SetStatic(false);
	gameObject2->AddComponent(collisionComp2);
	//gameObject2->SetActive(false);
	objManager->AddGameObject(gameObject2);

	//renderer.Resize(true, 1920, 1024);
	//renderer.Resize(false, 1280, 768);

	objManager->Init();
}

void GameState::Update()
{
	if (firstRun)
	{
		firstRun = false;
		return;
	}

	// input
	if (input.GetKeyPress('U'))
	{
		std::cout << "U is pressed" << std::endl;
		Restart(nullptr);
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

#ifdef _DEBUG
	debugRenderer.Add_Frustum(((CameraComponent*)objManager->GetGameObject(2)->GetComponent(0))->GetFrustum(), DirectX::XMFLOAT4(0, 1, 0, 1));
#endif

	// collision
	objManager->CollisionUpdate(collision);

	// update
	objManager->Update();

	// render
	renderer->Update(objManager);
}

void GameState::AddCamera(GameObject * cameraObject) {
	// load main camera
	CameraComponent * cameraComponent = new CameraComponent();
	cameraComponent->Init(UEngine::ComponentType_CAMERA, true, cameraObject);
	cameraComponent->Init(renderer, DirectX::XMFLOAT4(0, 0, 1, 1));
	cameraObject->AddComponent(cameraComponent);
}
void GameState::Restart(const char * sceneName) {
	if (collision)
	{
		delete collision;
		collision = nullptr;
	}
	if (objManager)
	{
		delete objManager;
		objManager = nullptr;
	}
	if (renderer)
	{
		delete renderer;
		renderer = nullptr;
	}
	renderer = new Renderer();
	objManager = new ObjectManager();
	collision = new CollisionManager();
	Init();
}