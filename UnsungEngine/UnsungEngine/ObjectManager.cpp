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
	for each (std::pair<int, GameObject*> obj in objs)
	{
		if (obj.second)
		{
			// update
		}
	}
}

void ObjectManager::Render(UVector<UEngine::RenderToTexture> & m_pRTT,
	UVector<Microsoft::WRL::ComPtr<ID3D11DeviceContext>> & m_pWorldDeferredContext,
	UVector<Microsoft::WRL::ComPtr<ID3D11CommandList>> & m_pWorldCommandList, Renderer * render) {
	std::vector<std::thread> threads;
	threads.push_back(std::thread([&]() {
		for each (std::pair<int, GameObject*> obj in objs)
		{
			if (obj.second)
			{
				obj.second->GetRenderComponent()->DrawObj(render);
				// Create command lists and record commands into them.
			}
		}
		for (unsigned i = 0; i < (unsigned)UEngine::DrawType_COUNT; i++)
		{
			m_pWorldDeferredContext[i]->FinishCommandList(false, m_pWorldCommandList[i].GetAddressOf());
		}
	}));
	for (auto& thread : threads)
		thread.join();
}

void ObjectManager::Clear()
{
	for each (std::pair<int, GameObject*> obj in objs)
	{
		if (obj.second)
			delete obj.second;
	}
	objs.clear();
}

void ObjectManager::AddObj(GameObject * _obj)
{
	if (_obj)
	{
		while (objs[obj_reference_num])
			obj_reference_num++;

		std::cout << obj_reference_num << std::endl;
		_obj->SetReferenceNum(obj_reference_num);
		objs[obj_reference_num] = _obj;
	}
	else {
		std::cout << "Object trying to add doesn't exist" << std::endl;
	}
}

void ObjectManager::RemoveObj(GameObject * _obj)
{
	unsigned refNum = _obj->GetReferenceNum();
	if (objs[refNum])
	{
		objs[refNum] = nullptr;
		delete _obj;
	}
	objs.erase(refNum);
}
