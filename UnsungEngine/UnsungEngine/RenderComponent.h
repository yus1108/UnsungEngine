#pragma once
#include "Renderer.h"
#include "Transform.h"
#include "Component.h"
#include "CollisionBox.h"

class Renderer;
class RenderComponent
{
protected:
	UEngine::pipeline_state_t * m_pPipeline;
	UEngine::DrawType type;

	GameObject * parent;
	CollisionBox * collisionBox;
	bool loadingDone;
	bool isActive;
	bool isInFrustum;
public:
	RenderComponent();
	virtual ~RenderComponent();

	bool GetActive() { return isActive; }
	void SetActive(bool _active) { isActive = _active; }
	UEngine::DrawType GetType() { return type; }
	void SetType(UEngine::DrawType _type) { type = _type; }
	CollisionBox * GetCollisionBox() { return collisionBox; }

	UEngine::pipeline_state_t * GetPipeline() { return m_pPipeline; }
	void SetPipeline(UEngine::pipeline_state_t * _pipeline) { m_pPipeline = _pipeline; }

	virtual void Init(UEngine::pipeline_state_t * pipeline, GameObject * _parent) = 0;
	virtual void Update() = 0;
	virtual void DrawObj(Renderer * render, Component * m_pCamera) = 0;
	virtual void ReadBin(const char * filename, ID3D11Device * m_pDevice,
		ID3D11DeviceContext * m_pDeviceContext, DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1, 1, 1, 1)) = 0;
	virtual void CalculateCBox() = 0;

};

