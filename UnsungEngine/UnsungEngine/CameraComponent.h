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
public:
	CameraComponent();
	~CameraComponent();

	D3D11_VIEWPORT * GetViewport() { return m_pViewport; }
	UEngine::RenderToTexture * GetRTTWorld() { return m_pRTTWorld; }
	UEngine::RenderToTexture * GetRTTUI() { return m_pRTTUI; }
	void SetViewport(D3D11_VIEWPORT * _viewport) { m_pViewport = _viewport; }
	void SetRTTWorld(UEngine::RenderToTexture * _rtt) { m_pRTTWorld = _rtt; }
	void SetRTTUI(UEngine::RenderToTexture * _rtt) { m_pRTTUI = _rtt; }
	ID3D11DeviceContext * GetDeferredContext(unsigned i) { return m_pDeferredContext[i].Get(); }
	ID3D11CommandList ** GetCommandList(unsigned i) { return m_pCommandList[i].GetAddressOf(); }
	void ReleaseCommandList(unsigned i) { m_pCommandList[i].ReleaseAndGetAddressOf(); }

	void SetAspectRatio(float r) { aspectRatio = r; };
	float GetAspectRatio() { return aspectRatio; };
	void SetAngle(float a) { angle = a; };
	float GetAngle() { return angle; };
	float GetNearZ() { return nearZ; };
	float GetFarZ() { return farZ; };
	void SetOriginalView(DirectX::XMMATRIX o) { originalView = o; forwardRotation.r[3] = o.r[3]; };
	DirectX::XMMATRIX GetOriginalView() { return originalView; };
	DirectX::XMMATRIX GetForwrdRotation() { return forwardRotation; };
	DirectX::XMMATRIX * GetAddrOriginalView() { return &originalView; };
	void SetSceneToShader(SCENE s) { sceneToShader = s; };
	SCENE GetSceneToShader() { return sceneToShader; };

	void Init(UEngine::ComponentType _type, bool _active, GameObject * _parent);
	void Init(Renderer * renderer);

	void CreateNewDeferredContext(ID3D11Device * m_pDevice);
	void Clear();

	void LookAt(DirectX::XMVECTOR targetPos);
	void ForwardLookAt(DirectX::XMVECTOR targetPos);
	void TurnTo(DirectX::XMVECTOR targetPos, float speed);
	void MouseLook(POINT mousePos, float speed);
};

