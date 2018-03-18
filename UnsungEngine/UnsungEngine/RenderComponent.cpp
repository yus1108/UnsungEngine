#include "stdafx.h"
#include "RenderComponent.h"


RenderComponent::RenderComponent()
{
	loadingDone = false;
	isActive = true;
}


RenderComponent::~RenderComponent()
{
}

void RenderComponent::Init(ID3D11DeviceContext * deviceContext, UEngine::pipeline_state_t * pipeline, UEngine::RenderToTexture * rtt, D3D11_VIEWPORT * viewport)
{
	m_pDeviceContext = deviceContext;
	m_pPipeline = pipeline;
	m_pRTT = rtt;
	m_viewport = viewport;
}
