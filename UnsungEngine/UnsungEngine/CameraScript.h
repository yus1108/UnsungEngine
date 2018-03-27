#pragma once
#include "ScriptComponent.h"

class CameraScript : public ScriptComponent
{
public:
	CameraScript();
	virtual ~CameraScript();

	void FrameUpdate();
};

