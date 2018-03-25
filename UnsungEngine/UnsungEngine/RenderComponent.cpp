#include "stdafx.h"
#include "RenderComponent.h"


RenderComponent::RenderComponent()
{
	loadingDone = false;
	isActive = true;
	isInFrustum = false;
	collisionBox = nullptr;
}


RenderComponent::~RenderComponent()
{
	if (collisionBox)
		delete collisionBox;
}

void RenderComponent::Init(UEngine::pipeline_state_t * pipeline)
{
	m_pPipeline = pipeline;
}
