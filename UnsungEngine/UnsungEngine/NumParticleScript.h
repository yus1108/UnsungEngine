#pragma once
#include "ScriptComponent.h"
class NumParticleScript :
	public ScriptComponent
{
public:
	NumParticleScript();
	~NumParticleScript();

	void FrameUpdate();
};

