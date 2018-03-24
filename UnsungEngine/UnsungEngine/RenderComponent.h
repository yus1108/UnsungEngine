#pragma once
#include "Renderer.h"
#include "Transform.h"
#include "Component.h"

class Renderer;
class RenderComponent
{
protected:
	UEngine::pipeline_state_t * m_pPipeline;
	UEngine::DrawType type;

	bool loadingDone;
	bool isActive;
public:
	RenderComponent();
	virtual ~RenderComponent();

	bool GetActive() { return isActive; }
	void SetActive(bool _active) { isActive = _active; }
	UEngine::DrawType GetType() { return type; }
	void SetType(UEngine::DrawType _type) { type = _type; }

	UEngine::pipeline_state_t * GetPipeline() { return m_pPipeline; }
	void SetPipeline(UEngine::pipeline_state_t * _pipeline) { m_pPipeline = _pipeline; }

	virtual void Init(ID3D11Device * m_pDevice, const WCHAR * textString, UINT32 textLength,
		const WCHAR * msc_fontName, const FLOAT msc_fontSize,
		UEngine::TextFormat textFormat = UEngine::TextFormat()) = 0;
	virtual void Init(UEngine::pipeline_state_t * pipeline) = 0;
	virtual void Update(Transform * transform) = 0;
	virtual void DrawObj(Renderer * render, Transform * transform, Component * m_pCamera) = 0;
	virtual void ReadBin(const char * filename, ID3D11Device * m_pDevice,
		ID3D11DeviceContext * m_pDeviceContext, DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1, 1, 1, 1)) = 0;


};

