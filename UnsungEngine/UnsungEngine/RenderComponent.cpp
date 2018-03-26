#include "stdafx.h"
#include "RenderComponent.h"


RenderComponent::RenderComponent()
{
	loadingDone = false;
	isActive = true;
	isInFrustum = false;
	parent = nullptr;
	collisionBox = nullptr;
}


RenderComponent::~RenderComponent()
{
	if (collisionBox)
		delete collisionBox;
}

void RenderComponent::Init(UEngine::pipeline_state_t * pipeline, GameObject * _parent)
{
	m_pPipeline = pipeline;
	parent = _parent;
}
