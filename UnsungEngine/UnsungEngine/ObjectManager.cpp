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

void ObjectManager::Render(UVector<Microsoft::WRL::ComPtr<ID3D11DeviceContext>> & m_pWorldDeferredContext,
	UVector<Microsoft::WRL::ComPtr<ID3D11CommandList>> & m_pWorldCommandList, Renderer * render) {
	std::vector<std::thread> threads;
	threads.push_back(std::thread([&]() {
		// Set the index buffer.
		for each (std::pair<int, GameObject*> obj in objs)
		{
			if (obj.second)
			{
				obj.second->GetRenderComponent()->DrawObj(render);
				// Create command lists and record commands into them.
				m_pWorldDeferredContext[obj.second->GetDrawType()]->FinishCommandList(false, m_pWorldCommandList[obj.second->GetDrawType()].GetAddressOf());
			}
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
