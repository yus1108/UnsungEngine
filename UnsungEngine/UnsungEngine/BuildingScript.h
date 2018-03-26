#pragma once
#include "ScriptComponent.h"

class BuildingScript : public ScriptComponent
{
public:
	BuildingScript();
	virtual ~BuildingScript();

	void FrameUpdate();
};

