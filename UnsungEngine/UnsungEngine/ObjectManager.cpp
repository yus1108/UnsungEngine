#include "stdafx.h"
#include "ObjectManager.h"


ObjectManager::ObjectManager()
{
}
ObjectManager::~ObjectManager()
{
	Clear();
}

void ObjectManager::Init() {
	obj_reference_num = 0;
}

void ObjectManager::Update() {
	for each (std::pair<int, GameObject*> obj in gameObjects)
	{
		if (obj.second)
		{
			// update
			obj.second->Update();
		}
	}
}

void ObjectManager::Render(UVector<UEngine::RenderToTexture> & m_pRTT,
	UVector<Microsoft::WRL::ComPtr<ID3D11DeviceContext>> & m_pWorldDeferredContext,
	UVector<Microsoft::WRL::ComPtr<ID3D11CommandList>> & m_pWorldCommandList, Renderer * render) {
	std::vector<std::thread> threads;
	threads.push_back(std::thread([&]() {
		for each (std::pair<int, GameObject*> obj in gameObjects)
		{
			if (obj.second)
			{
				obj.second->GetRenderComponent()->DrawObj(render, obj.second->GetTransform());
				// Create command lists and record commands into them.
			}
		}
		for (unsigned i = 0; i < (unsigned)UEngine::DrawType_COUNT; i++)
		{
			m_pWorldDeferredContext[i]->FinishCommandList(true, m_pWorldCommandList[i].GetAddressOf());
		}
	}));
	for (auto& thread : threads)
		thread.join();
}

void ObjectManager::Clear()
{
	for each (std::pair<int, GameObject*> obj in gameObjects)
	{
		if (obj.second)
			delete obj.second;
	}
	gameObjects.clear();
}

GameObject * ObjectManager::GetGameObject(unsigned i)
{
	if (gameObjects[i])
		return gameObjects[i];

	gameObjects.erase(i);
	return nullptr;
}
void ObjectManager::AddGameObject(GameObject * _obj)
{
	if (_obj)
	{
		unsigned counter = 0;
		while (gameObjects[obj_reference_num])
		{
			obj_reference_num++;
			counter++;
			if (counter == UINT_MAX)
			{
				std::cout << "Cannot create more than the space provided in the container!" << std::endl;
				return;
			}
		}

		std::cout << "object number " << obj_reference_num << " created." << std::endl;
		_obj->SetReferenceNum(obj_reference_num);
		gameObjects[obj_reference_num] = _obj;
	}
	else {
		std::cout << "Object trying to add doesn't exist" << std::endl;
	}
}
void ObjectManager::RemoveGameObject(GameObject * _obj)
{
	unsigned refNum = _obj->GetReferenceNum();
	if (gameObjects[refNum])
	{
		std::cout << "object number " << refNum << " deleted." << std::endl;
		gameObjects[refNum] = nullptr;
		delete _obj;
	}
	else
	{
		std::cout << "object number " << refNum << " doesn't exist." << std::endl;
	}
	gameObjects.erase(refNum);
}
