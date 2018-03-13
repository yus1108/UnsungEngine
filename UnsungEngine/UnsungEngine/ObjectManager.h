#pragma once
#include <unordered_map>
#include "GameObject.h"

class ObjectManager
{
	unsigned obj_reference_num;
	std::unordered_map<unsigned, GameObject*> gameObjects;
public:
	ObjectManager();
	~ObjectManager();

	void Init();
	void Update();
	void Render(UVector<UEngine::RenderToTexture>& m_pRTT, UVector<Microsoft::WRL::ComPtr<ID3D11DeviceContext>>& m_pWorldDeferredContext, UVector<Microsoft::WRL::ComPtr<ID3D11CommandList>>& m_pWorldCommandList, Renderer * render);
	void Clear();

	GameObject * GetGameObject(unsigned i);
	void AddGameObject(GameObject * _obj);
	void RemoveGameObject(GameObject * _obj);
};

