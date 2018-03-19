#pragma once
#include "Renderer.h"
#include "Component.h"

class CameraComponent : public Component
{
private:
	float aspectRatio;
	float angle;
	DirectX::XMMATRIX originalView;
	DirectX::XMMATRIX forwardRotation;
	SCENE sceneToShader;
	float nearZ;
	float farZ;
	//Trapezoid frustum;

	D3D11_VIEWPORT * m_pViewport;
	UEngine::RenderToTexture * m_pRTTWorld;
	UEngine::RenderToTexture * m_pRTTUI;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pDeferredContext[2];
	Microsoft::WRL::ComPtr<ID3D11CommandList> m_pCommandList[2];
	Renderer * m_pRenderer;
	DirectX::XMFLOAT4 viewRatio;
public:
	CameraComponent();
	~CameraComponent();

	D3D11_VIEWPORT * GetViewport() { return m_pViewport; }
	void SetViewport(D3D11_VIEWPORT * _viewport) { m_pViewport = _viewport; }
	UEngine::RenderToTexture * GetRTTWorld() { return m_pRTTWorld; }
	void SetRTTWorld(UEngine::RenderToTexture * _rtt) { m_pRTTWorld = _rtt; }
	UEngine::RenderToTexture * GetRTTUI() { return m_pRTTUI; }
	void SetRTTUI(UEngine::RenderToTexture * _rtt) { m_pRTTUI = _rtt; }

	DirectX::XMFLOAT4 GetViewRatio() { return viewRatio; }
	void SetViewRatio(DirectX::XMFLOAT4 _ratio) { viewRatio = _ratio; }

	ID3D11DeviceContext * GetDeferredContext(unsigned i) { return m_pDeferredContext[i].Get(); }
	ID3D11CommandList ** GetCommandList(unsigned i) { return m_pCommandList[i].GetAddressOf(); }
	void ReleaseCommandList(unsigned i) { m_pCommandList[i].ReleaseAndGetAddressOf(); }

	float GetAspectRatio() { return aspectRatio; };
	void SetAspectRatio(float r) { aspectRatio = r; };
	float GetAngle() { return angle; };
	void SetAngle(float a) { angle = a; };
	float GetNearZ() { return nearZ; };
	float GetFarZ() { return farZ; };
	DirectX::XMMATRIX GetOriginalView() { return originalView; };
	void SetOriginalView(DirectX::XMMATRIX o) { originalView = o; forwardRotation.r[3] = o.r[3]; };
	DirectX::XMMATRIX GetForwrdRotation() { return forwardRotation; };
	DirectX::XMMATRIX * GetAddrOriginalView() { return &originalView; };
	SCENE GetSceneToShader() { return sceneToShader; };
	void SetSceneToShader(SCENE s) { sceneToShader = s; };

	void Init(UEngine::ComponentType _type, bool _active, GameObject * _parent);
	/*
	Function name: Init
	param: renderer - rendering class
	param: viewRatio - size ratio of camera view relative to the main render target (0 - 1)(x = left, y = top, z = right, w = bottom)
	return: void
	*/
	void Init(Renderer * renderer, DirectX::XMFLOAT4 viewRatio);

	void CreateNewDeferredContext(ID3D11Device * m_pDevice);
	void Clear();

	void LookAt(DirectX::XMVECTOR targetPos);
	void ForwardLookAt(DirectX::XMVECTOR targetPos);
	void TurnTo(DirectX::XMVECTOR targetPos, float speed);
	void MouseLook(POINT mousePos, float speed);
};