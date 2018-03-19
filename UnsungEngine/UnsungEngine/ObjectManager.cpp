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
		if (obj.second && obj.second->GetActive())
		{
			// update
			obj.second->Update();
		}
	}
}

void ObjectManager::Render(CameraComponent * m_pCamera, Renderer * render) {
#pragma region CLEAR_BUFFER
	// clearing world
	m_pCamera->GetDeferredContext(0)->OMSetDepthStencilState(m_pCamera->GetRTTWorld()->depthStencilState.Get(), 1);
	m_pCamera->GetDeferredContext(0)->OMSetRenderTargets(1, m_pCamera->GetRTTWorld()->renderTargetViewMap.GetAddressOf(),
		m_pCamera->GetRTTWorld()->depthStencilView.Get());
	m_pCamera->GetDeferredContext(0)->ClearRenderTargetView(m_pCamera->GetRTTWorld()->renderTargetViewMap.Get(), DirectX::Colors::Transparent);
	m_pCamera->GetDeferredContext(0)->ClearDepthStencilView(m_pCamera->GetRTTWorld()->depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	// clearing ui
	m_pCamera->GetDeferredContext(1)->OMSetDepthStencilState(m_pCamera->GetRTTUI()->depthStencilState.Get(), 1);
	m_pCamera->GetDeferredContext(1)->OMSetRenderTargets(1, m_pCamera->GetRTTUI()->renderTargetViewMap.GetAddressOf(),
		m_pCamera->GetRTTWorld()->depthStencilView.Get());
	m_pCamera->GetDeferredContext(1)->ClearRenderTargetView(m_pCamera->GetRTTUI()->renderTargetViewMap.Get(), DirectX::Colors::Transparent);
	m_pCamera->GetDeferredContext(1)->ClearDepthStencilView(m_pCamera->GetRTTUI()->depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
#pragma endregion

	for each (std::pair<int, GameObject*> obj in gameObjects)
	{
		if (obj.second && obj.second->GetActive())
		{
			if (obj.second->GetRenderComponent())
			{
				unsigned drawType = (unsigned)obj.second->GetRenderComponent()->GetType();
#pragma region RENDER_SET
				switch (drawType)
				{
				case UEngine::DrawType_WORLD:
					m_pCamera->GetDeferredContext(drawType)->OMSetDepthStencilState(m_pCamera->GetRTTWorld()->depthStencilState.Get(), 1);
					m_pCamera->GetDeferredContext(drawType)->OMSetRenderTargets(1,
						m_pCamera->GetRTTWorld()->renderTargetViewMap.GetAddressOf(),
						m_pCamera->GetRTTWorld()->depthStencilView.Get());
					break;
				case UEngine::DrawType_UI:
					m_pCamera->GetDeferredContext(drawType)->OMSetDepthStencilState(m_pCamera->GetRTTUI()->depthStencilState.Get(), 1);
					m_pCamera->GetDeferredContext(drawType)->OMSetRenderTargets(1,
						m_pCamera->GetRTTUI()->renderTargetViewMap.GetAddressOf(),
						m_pCamera->GetRTTUI()->depthStencilView.Get());
					break;
				default:
					break;
				}
				m_pCamera->GetDeferredContext(drawType)->OMSetBlendState(
					obj.second->GetRenderComponent()->GetPipeline()->blendingState.Get(), NULL, 0xffffffff);
				m_pCamera->GetDeferredContext(drawType)->RSSetViewports(1, m_pCamera->GetViewport());

				// Bind depth stencil state
				m_pCamera->GetDeferredContext(drawType)->RSSetState(obj.second->GetRenderComponent()->GetPipeline()->rasterState.Get());
				ID3D11SamplerState *sampler[]{ obj.second->GetRenderComponent()->GetPipeline()->samplerState.Get() };
				m_pCamera->GetDeferredContext(drawType)->PSSetSamplers(0, 1, sampler);

				// set shaders and input layout
				m_pCamera->GetDeferredContext(drawType)->VSSetShader(obj.second->GetRenderComponent()->GetPipeline()->vertex_shader.Get(), nullptr, 0);
				m_pCamera->GetDeferredContext(drawType)->PSSetShader(obj.second->GetRenderComponent()->GetPipeline()->pixel_shader.Get(), nullptr, 0);
				m_pCamera->GetDeferredContext(drawType)->GSSetShader(obj.second->GetRenderComponent()->GetPipeline()->geometry_shader.Get(), nullptr, 0);
				m_pCamera->GetDeferredContext(drawType)->IASetInputLayout(obj.second->GetRenderComponent()->GetPipeline()->input_layout.Get());
				switch (drawType)
				{
				case UEngine::DrawType_WORLD:
					m_pCamera->GetDeferredContext(drawType)->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
					break;
				case UEngine::DrawType_UI:
					m_pCamera->GetDeferredContext(drawType)->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
					break;
				default:
					break;
				}
#pragma endregion
				obj.second->GetRenderComponent()->DrawObj(render, obj.second->GetTransform(), m_pCamera);
			}
		}
	}
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
