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

void RenderComponent::Init(UEngine::pipeline_state_t * pipeline)
{
	m_pPipeline = pipeline;
}
