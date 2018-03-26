#include "stdafx.h"
#include "FrameTextScript.h"


FrameTextScript::FrameTextScript()
{
}


FrameTextScript::~FrameTextScript()
{
}

void FrameTextScript::FrameUpdate()
{
	if (parentObject->GetActive())
	{
		std::stringstream stringBuilder;
		stringBuilder << "FPS: " << (unsigned)utime.FramePerSecond() << std::endl;
		char pch[20];
		stringBuilder.getline(pch, 20);
		gameState.renderer.ChangeGUI(pch, parentObject);
	}
}
