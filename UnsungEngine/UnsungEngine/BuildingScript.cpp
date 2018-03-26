#include "stdafx.h"
#include "BuildingScript.h"


BuildingScript::BuildingScript()
{
}


BuildingScript::~BuildingScript()
{
}

void BuildingScript::FrameUpdate()
{
	if (parentObject->GetActive())
	{
		DirectX::XMMATRIX worldMat = DirectX::XMMatrixMultiply(parentObject->GetTransform()->GetMatrix(), DirectX::XMMatrixRotationY((float)utime.DeltaTime() / 10.0f));
		parentObject->GetTransform()->SetMatrix(worldMat);
	}
}
