#pragma once
#include "ScriptComponent.h"
class FrameTextScript :
	public ScriptComponent
{
public:
	FrameTextScript();
	virtual ~FrameTextScript();

	void FrameUpdate();
};

