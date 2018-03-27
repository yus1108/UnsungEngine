#include "stdafx.h"
#include "CameraScript.h"


CameraScript::CameraScript()
{
}


CameraScript::~CameraScript()
{
}

void CameraScript::FrameUpdate()
{
	CameraComponent * camera = (CameraComponent*)parentObject->GetComponent(0);
	if (input.GetKey('W')) {
		DirectX::XMMATRIX view = camera->GetForwrdRotation();
		view = XMMatrixMultiply(DirectX::XMMatrixTranslation(0, 0, (float)utime.DeltaTime() * 20), view);
		DirectX::XMMATRIX oView = camera->GetOriginalView();
		oView.r[3] = view.r[3];
		camera->SetOriginalView(oView);
	}
	if (input.GetKey('A')) {
		DirectX::XMMATRIX view = camera->GetForwrdRotation();
		view = DirectX::XMMatrixMultiply(DirectX::XMMatrixTranslation((float)-utime.DeltaTime() * 20, 0, 0), view);
		DirectX::XMMATRIX oView = camera->GetOriginalView();
		oView.r[3] = view.r[3];
		camera->SetOriginalView(oView);
	}
	if (input.GetKey('S')) {
		DirectX::XMMATRIX view = camera->GetForwrdRotation();
		view = DirectX::XMMatrixMultiply(DirectX::XMMatrixTranslation(0, 0, (float)-utime.DeltaTime() * 20), view);
		DirectX::XMMATRIX oView = camera->GetOriginalView();
		oView.r[3] = view.r[3];
		camera->SetOriginalView(oView);
	}
	if (input.GetKey('D')) {
		DirectX::XMMATRIX view = camera->GetForwrdRotation();
		view = DirectX::XMMatrixMultiply(DirectX::XMMatrixTranslation((float)utime.DeltaTime() * 20, 0, 0), view);
		DirectX::XMMATRIX oView = camera->GetOriginalView();
		oView.r[3] = view.r[3];
		camera->SetOriginalView(oView);
	}
	if (input.GetKey('E')) {
		DirectX::XMMATRIX view = camera->GetOriginalView();
		view = DirectX::XMMatrixMultiply(DirectX::XMMatrixTranslation(0, (float)utime.DeltaTime() * 20, 0), view);
		parentObject->GetTransform()->SetMatrix(view);
		camera->SetOriginalView(view);
	}
	if (input.GetKey('Q')) {
		DirectX::XMMATRIX view = camera->GetOriginalView();
		view = DirectX::XMMatrixMultiply(DirectX::XMMatrixTranslation(0, (float)-utime.DeltaTime() * 20, 0), view);
		camera->SetOriginalView(view);
	}
	if (input.GetMouseInput(UEngine::MouseInputType_MIDDLE))
		camera->MouseLook(input.PrevMousePos, 0.005f);
	GetCursorPos(&input.PrevMousePos);
}
