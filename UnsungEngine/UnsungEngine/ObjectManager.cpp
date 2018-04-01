#include "stdafx.h"
#include "ObjectManager.h"


ObjectManager::ObjectManager()
{
	obj_reference_num = 0;
}
ObjectManager::~ObjectManager()
{
	Clear();
}

void ObjectManager::Init() {
	for each (std::pair<int, GameObject*> obj in gameObjects)
	{
		if (obj.second && obj.second->GetActive())
		{
			// update
			obj.second->Init();
		}
	}
}

void ObjectManager::CollisionUpdate(CollisionManager * collision)
{
	collision->Clear(collision->GetRoot());
	for each (std::pair<int, GameObject*> obj in gameObjects)
	{
		if (obj.second && obj.second->GetActive())
		{
			// collision update
			for (unsigned i = 0; i < obj.second->GetSizeComponents(); i++)
			{
				Component * component = obj.second->GetComponent(i);
				if (component->GetType() == UEngine::ComponentType_COLLISION)
				{
					collision->Update((CollisionComponent*)component);
				}
			}
		}
	}
	
}

void ObjectManager::Update() {
	for each (std::pair<int, GameObject*> obj in gameObjects)
	{
		if (obj.second && obj.second->GetActive())
		{
			// update
			obj.second->GetTransform()->SetPrevPos();
			obj.second->Update();
		}
	}
}

void ObjectManager::Render(CameraComponent * m_pCamera, Renderer * render) {
#pragma region CLEAR_BUFFER
	UVector<int> threads;
	threads.push_back(threadPool.AddTask([&]() {
		// clearing world
		m_pCamera->GetDeferredContext(0)->OMSetDepthStencilState(m_pCamera->GetRTTWorld()->depthStencilState.Get(), 1);
		m_pCamera->GetDeferredContext(0)->OMSetRenderTargets(1, m_pCamera->GetRTTWorld()->renderTargetViewMap.GetAddressOf(),
			m_pCamera->GetRTTWorld()->depthStencilView.Get());
		m_pCamera->GetDeferredContext(0)->ClearRenderTargetView(m_pCamera->GetRTTWorld()->renderTargetViewMap.Get(), DirectX::Colors::Transparent);
		m_pCamera->GetDeferredContext(0)->ClearDepthStencilView(m_pCamera->GetRTTWorld()->depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}));
	threads.push_back(threadPool.AddTask([&]() {
		// clearing ui
		m_pCamera->GetDeferredContext(1)->OMSetDepthStencilState(m_pCamera->GetRTTUI()->depthStencilState.Get(), 1);
		m_pCamera->GetDeferredContext(1)->OMSetRenderTargets(1, m_pCamera->GetRTTUI()->renderTargetViewMap.GetAddressOf(),
			m_pCamera->GetRTTUI()->depthStencilView.Get());
		m_pCamera->GetDeferredContext(1)->ClearRenderTargetView(m_pCamera->GetRTTUI()->renderTargetViewMap.Get(), DirectX::Colors::Transparent);
		m_pCamera->GetDeferredContext(1)->ClearDepthStencilView(m_pCamera->GetRTTUI()->depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}));
	D3D11_VIEWPORT viewport = *m_pCamera->GetViewport();
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	for (unsigned i = 0; i < threads.size(); i++)
		threadPool.Join(threads[i]);
	threads.clear();
#pragma endregion

	for each (std::pair<int, GameObject*> obj in gameObjects)
	{
		if (obj.second && obj.second->GetActive())
		{
			if (obj.second->GetRenderComponent())
			{
				DrawObj(m_pCamera, render, viewport, obj.second);
			}
		}
	}
}

void ObjectManager::DrawObj(CameraComponent * m_pCamera, Renderer * render, D3D11_VIEWPORT viewport, GameObject * obj) {
	unsigned drawType = (unsigned)obj->GetRenderComponent()->GetType();
	if (drawType == UEngine::DrawType_UI && ((Render_UI*)obj->GetRenderComponent())->GetCamera() != m_pCamera->GetParent()->GetReferenceNum())
		return;

	ID3D11DeviceContext * deferredContext = m_pCamera->GetDeferredContext(drawType);
#pragma region RENDER_SET
	switch (drawType)
	{
	case UEngine::DrawType_WORLD:
		deferredContext->OMSetDepthStencilState(m_pCamera->GetRTTWorld()->depthStencilState.Get(), 1);
		deferredContext->OMSetRenderTargets(1,
			m_pCamera->GetRTTWorld()->renderTargetViewMap.GetAddressOf(),
			m_pCamera->GetRTTWorld()->depthStencilView.Get());
		break;
	case UEngine::DrawType_UI:
		deferredContext->OMSetDepthStencilState(m_pCamera->GetRTTUI()->depthStencilState.Get(), 1);
		deferredContext->OMSetRenderTargets(1,
			m_pCamera->GetRTTUI()->renderTargetViewMap.GetAddressOf(),
			m_pCamera->GetRTTUI()->depthStencilView.Get());
		break;
	default:
		break;
	}
	deferredContext->OMSetBlendState(
		obj->GetRenderComponent()->GetPipeline()->blendingState.Get(), NULL, 0xffffffff);
	deferredContext->RSSetViewports(1, &viewport);

	// Bind depth stencil state
	deferredContext->RSSetState(obj->GetRenderComponent()->GetPipeline()->rasterState.Get());
	ID3D11SamplerState *sampler[]{ obj->GetRenderComponent()->GetPipeline()->samplerState.Get() };
	deferredContext->PSSetSamplers(0, 1, sampler);

	// set shaders and input layout
	deferredContext->VSSetShader(obj->GetRenderComponent()->GetPipeline()->vertex_shader.Get(), nullptr, 0);
	deferredContext->PSSetShader(obj->GetRenderComponent()->GetPipeline()->pixel_shader.Get(), nullptr, 0);
	deferredContext->GSSetShader(obj->GetRenderComponent()->GetPipeline()->geometry_shader.Get(), nullptr, 0);
	deferredContext->IASetInputLayout(obj->GetRenderComponent()->GetPipeline()->input_layout.Get());
	switch (drawType)
	{
	case UEngine::DrawType_WORLD:
		deferredContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		break;
	case UEngine::DrawType_UI:
		deferredContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		break;
	default:
		break;
	}
#pragma endregion
	obj->GetRenderComponent()->DrawObj(render, m_pCamera);
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
