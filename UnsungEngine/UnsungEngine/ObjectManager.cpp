#include "stdafx.h"
#include "ObjectManager.h"


ObjectManager::ObjectManager()
{
}


ObjectManager::~ObjectManager()
{
}

void ObjectManager::Init() {
	obj_reference_num = 0;
}

void ObjectManager::Update() {

}

void ObjectManager::Render(UVector<Microsoft::WRL::ComPtr<ID3D11DeviceContext>> & m_pWorldDeferredContext,
	UVector<Microsoft::WRL::ComPtr<ID3D11CommandList>> & m_pWorldCommandList, Renderer * render) {
	std::vector<std::thread> threads;
	threads.push_back(std::thread([&]() {
		// Set the index buffer.
		for (unsigned i = 0; i < objs.size(); i++)
		{
			objs[i]->GetRenderComponent()->DrawObj(render);
			// Create command lists and record commands into them.
			m_pWorldDeferredContext[objs[i]->GetDrawType()]->FinishCommandList(false, m_pWorldCommandList[objs[i]->GetDrawType()].GetAddressOf());
		}
	}));
	for (auto& thread : threads)
		thread.join();
}

void ObjectManager::AddObj(GameObject * _obj)
{
	if (_obj)
	{
		while (objsMap[obj_reference_num])
		{
			std::cout << obj_reference_num << std::endl;
			obj_reference_num++;
		}

		objs.push_back(_obj);
		objsMap[obj_reference_num] = _obj;
	}
	else {
		std::cout << "Object trying to add doesn't exist" << std::endl;
	}
}
