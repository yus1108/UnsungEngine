#pragma once
#include "ScriptComponent.h"

class NumParticleScript :
	public ScriptComponent
{
public:
	NumParticleScript();
	virtual ~NumParticleScript();

	void FrameUpdate();
};

