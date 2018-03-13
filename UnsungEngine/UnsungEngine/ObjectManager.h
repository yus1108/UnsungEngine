#pragma once
#include <unordered_map>
#include "GameObject.h"

class ObjectManager
{
	unsigned obj_reference_num;
	std::unordered_map<unsigned, GameObject*> objsMap;
	UVector<GameObject*> objs;
public:
	ObjectManager();
	~ObjectManager();
	void Init();
	void Update();
	void Render(UVector<Microsoft::WRL::ComPtr<ID3D11DeviceContext>>& m_pWorldDeferredContext, 
		UVector<Microsoft::WRL::ComPtr<ID3D11CommandList>>& m_pWorldCommandList, 
		Renderer * render);

	void AddObj(GameObject * _obj);
	void RemoveObj(GameObject * _obj);
};

