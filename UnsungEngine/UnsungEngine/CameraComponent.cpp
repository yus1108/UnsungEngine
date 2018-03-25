#include "stdafx.h"
#include "CameraComponent.h"
using namespace DirectX;

CameraComponent::CameraComponent() : Component()
{
	m_pViewport = nullptr;
	m_pRTTWorld = nullptr;
	m_pRTTUI = nullptr;
}
CameraComponent::~CameraComponent()
{
	for (unsigned i = 0; i < m_pRenderer->GetSizeCameras(); i++)
	{
		if (m_pRenderer->GetCameras(i) == this)
		{
			m_pRenderer->RemoveCameras(i);
			break;
		}
	}
	if (m_pViewport)
		delete m_pViewport;
	if (m_pRTTWorld)
		delete m_pRTTWorld;
	if (m_pRTTUI)
		delete m_pRTTUI;
}

void CameraComponent::Init(UEngine::ComponentType _type, bool _active, GameObject * _parent)
{
	Component::Init(_type, _active, _parent);
}
void CameraComponent::Init(Renderer * renderer, DirectX::XMFLOAT4 viewRatio)
{
	m_pRenderer = renderer;
	this->viewRatio = viewRatio;
	RECT clientSize;
	GetClientRect(hWnd, &clientSize);
	clientSize.left = (LONG)(clientSize.right * viewRatio.x);
	clientSize.top = (LONG)(clientSize.bottom * viewRatio.y);
	clientSize.right = (LONG)(clientSize.right * viewRatio.z);
	clientSize.bottom = (LONG)(clientSize.bottom * viewRatio.w);
	renderer->AddCameras(this, clientSize);

	// perspective view
	SetAngle(60 / 180.0f * 3.14159f);
	nearZ = 0.1f;
	farZ = 100;
	DirectX::XMMATRIX originalView = DirectX::XMMatrixIdentity();
	originalView.r[3] = DirectX::XMVectorSet(0, 5, -20, 1); //
	forwardRotation = DirectX::XMMatrixIdentity();
	SetOriginalView(originalView);

	aspectRatio = ((m_pViewport->Width) / (m_pViewport->Height));
	DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant(originalView);
	sceneToShader.viewMat = DirectX::XMMatrixInverse(&determinant, originalView);
	sceneToShader.viewMat = DirectX::XMMatrixTranspose(sceneToShader.viewMat);
	sceneToShader.perspectivMat = DirectX::XMMatrixPerspectiveFovLH(GetAngle(), aspectRatio, GetNearZ(), GetFarZ());
	sceneToShader.perspectivMat = DirectX::XMMatrixTranspose(sceneToShader.perspectivMat);
}

void CameraComponent::Update()
{
	DirectX::XMMATRIX originalView = GetOriginalView();
	DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant(originalView);
	float aspectRatio = ((GetViewport()->Width) / (GetViewport()->Height));
	SetAspectRatio(aspectRatio);
	SCENE sceneToShader;
	sceneToShader.viewMat = DirectX::XMMatrixInverse(&determinant, originalView);
	sceneToShader.viewMat = DirectX::XMMatrixTranspose(sceneToShader.viewMat);
	sceneToShader.perspectivMat = DirectX::XMMatrixPerspectiveFovLH(GetAngle(), aspectRatio, GetNearZ(), GetFarZ());
	sceneToShader.perspectivMat = DirectX::XMMatrixTranspose(sceneToShader.perspectivMat);
	SetSceneToShader(sceneToShader);
}

void CameraComponent::CreateNewDeferredContext(ID3D11Device * m_pDevice)
{
	m_pDevice->CreateDeferredContext(NULL, m_pDeferredContext[0].GetAddressOf());
	m_pDevice->CreateDeferredContext(NULL, m_pDeferredContext[1].GetAddressOf());
}
void CameraComponent::Clear() {
	if (m_pViewport)
		delete m_pViewport;
	if (m_pRTTWorld)
		delete m_pRTTWorld;
	if (m_pRTTUI)
		delete m_pRTTUI;
}

void CameraComponent::LookAt(DirectX::XMVECTOR targetPos)
{
	DirectX::XMVECTOR xAxis, yAxis, zAxis;

	zAxis = targetPos - originalView.r[3];
	zAxis = XMVector3Normalize(zAxis);
	if (abs(zAxis.m128_f32[0]) < 0.000001f && abs(zAxis.m128_f32[1]) < 0.000001f && abs(zAxis.m128_f32[2]) < 0.000001f)
		return;
	xAxis = XMVector3Cross(XMVectorSet(0, 1, 0, 0), zAxis);
	xAxis = XMVector3Normalize(xAxis);
	if (abs(xAxis.m128_f32[0]) < 0.000001f && abs(xAxis.m128_f32[1]) < 0.000001f && abs(xAxis.m128_f32[2]) < 0.000001f)
		return;
	yAxis = XMVector3Cross(zAxis, xAxis);
	yAxis = XMVector3Normalize(yAxis);
	if (abs(yAxis.m128_f32[0]) < 0.000001f && abs(yAxis.m128_f32[1]) < 0.000001f && abs(yAxis.m128_f32[2]) < 0.000001f)
		return;

	originalView = XMMATRIX(xAxis, yAxis, zAxis, originalView.r[3]);
}
void CameraComponent::ForwardLookAt(DirectX::XMVECTOR targetPos)
{
	DirectX::XMVECTOR xAxis, yAxis, zAxis;

	zAxis = targetPos - forwardRotation.r[3];
	zAxis = XMVector3Normalize(zAxis);
	if (abs(zAxis.m128_f32[0]) < 0.000001f && abs(zAxis.m128_f32[1]) < 0.000001f && abs(zAxis.m128_f32[2]) < 0.000001f)
		return;
	xAxis = XMVector3Cross(XMVectorSet(0, 1, 0, 0), zAxis);
	xAxis = XMVector3Normalize(xAxis);
	if (abs(xAxis.m128_f32[0]) < 0.000001f && abs(xAxis.m128_f32[1]) < 0.000001f && abs(xAxis.m128_f32[2]) < 0.000001f)
		return;
	yAxis = XMVector3Cross(zAxis, xAxis);
	yAxis = XMVector3Normalize(yAxis);
	if (abs(yAxis.m128_f32[0]) < 0.000001f && abs(yAxis.m128_f32[1]) < 0.000001f && abs(yAxis.m128_f32[2]) < 0.000001f)
		return;

	forwardRotation = XMMATRIX(xAxis, yAxis, zAxis, forwardRotation.r[3]);
}
void CameraComponent::TurnTo(DirectX::XMVECTOR targetPos, float speed) {
	XMVECTOR wPos, xAxis, yAxis, v;
	wPos = originalView.r[3];

	xAxis = originalView.r[0];
	v = targetPos - wPos;
	float value = XMVector3Dot(v, xAxis).m128_f32[0] * speed * 0.99f;
	XMMATRIX temp = originalView;
	temp = XMMatrixMultiply(XMMatrixTranslation(value, 0, 1), temp);

	yAxis = -originalView.r[1];
	value = XMVector3Dot(v, yAxis).m128_f32[0] * speed * 0.99f;
	temp = XMMatrixMultiply(XMMatrixTranslation(0, -value, 0), temp);
	LookAt(temp.r[3]);
}
void CameraComponent::MouseLook(POINT mousePos, float speed) {
	POINT currMousePos;
	GetCursorPos(&currMousePos);

	float deltaX = (float)currMousePos.x - mousePos.x;
	float deltaY = (float)currMousePos.y - mousePos.y;

	XMMATRIX temp = originalView;

	temp = XMMatrixMultiply(XMMatrixTranslation(speed * deltaX, -speed * deltaY, 1), temp);
	LookAt(temp.r[3]);

	forwardRotation = originalView;
	forwardRotation.r[2] = XMVector3Cross(forwardRotation.r[0], XMVectorSet(0, 1, 0, 0));
	forwardRotation.r[1] = XMVector3Cross(forwardRotation.r[2], forwardRotation.r[0]);
}