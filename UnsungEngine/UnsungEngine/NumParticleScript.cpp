#include "stdafx.h"
#include "NumParticleScript.h"


NumParticleScript::NumParticleScript()
{
}


NumParticleScript::~NumParticleScript()
{
}

void NumParticleScript::FrameUpdate()
{
	if (parentObject->GetActive())
	{
		std::stringstream stringBuilder;
		stringBuilder << "Particles: " << (unsigned)((Render_Particle*)gameState.objManager->GetGameObject(3)->GetRenderComponent())->GetNumParticles() << std::endl;
		char pch[20];
		stringBuilder.getline(pch, 20);
		gameState.renderer->ChangeGUI(pch, parentObject);
	}
}
