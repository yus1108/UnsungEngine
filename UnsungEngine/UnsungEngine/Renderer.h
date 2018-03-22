#pragma once
#include "GameObject.h"
#include "ObjectManager.h"
#include "CameraComponent.h"
#include "RenderComponent.h"

class Renderer
{
	friend class Render_World;
	friend class Render_UI;
	friend class Render_Particle;
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> default_vertexBuffer;
	UEngine::RenderToTexture default_RTT;
	UEngine::pipeline_state_t default_pipeline;
	D3D11_VIEWPORT default_viewport;

	DXGI_SWAP_CHAIN_DESC									m_pSwapchainDesc;
	Microsoft::WRL::ComPtr<IDXGISwapChain>					m_pSwapCahin;
	Microsoft::WRL::ComPtr<ID3D11Device>					m_pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>				m_pDeviceContext;

	UVector<UEngine::pipeline_state_t>	m_pPipelines;
	UVector<CameraComponent*> m_pCameras;

	ID3D11Buffer * constBufferWorld;
	ID3D11Buffer * constBufferScene;
	ID3D11Buffer * constBufferDLight;
	ID3D11Buffer * constBufferPLight;
	ID3D11Buffer * constBufferSLight;
	ID3D11Buffer * constBufferLightInfo;
	ID3D11Buffer * constBufferRTTPos;
	ID3D11Buffer * constBufferRTTSize;
	ID3D11Buffer * constBufferParticleWorld;

	bool loadingDone;
public:
	Renderer();
	~Renderer();

	void ClearCameras() { m_pCameras.clear(); }
	unsigned GetSizeCameras() { return m_pCameras.size(); }
	void AddCameras(CameraComponent * component, RECT clientSize);
	CameraComponent * GetCameras(unsigned i) { return m_pCameras[i]; }
	void RemoveCameras(unsigned i) { m_pCameras.erase(i); }

	void Init();
	void Update(ObjectManager * objManager);

	void DeferredRendering(ObjectManager * objManager, int i);
	void FinishComands(int i);

	void Resize(bool isFullScreen, int width, int height);
	void LoadObject(const char * name, GameObject * gameObject);
	void LoadGUI(const char * textureName, GameObject * gameObject);
	void LoadGUI(const WCHAR * inputString, unsigned length, GameObject * gameObject);
	void LoadParticle(const char * name, GameObject * gameObject);
	void ChangeGUI(const char * textStr, GameObject * gameObject, UEngine::TextFormat * textFormat = nullptr);
private:
	void RenderSet(ID3D11DeviceContext * m_pDeviceContext, UEngine::pipeline_state_t & pipeline, UEngine::RenderToTexture & rtt,
		D3D11_VIEWPORT & viewport, D3D11_PRIMITIVE_TOPOLOGY topology);
	void DebugSet(UEngine::pipeline_state_t * pipeline);

	// initialization functions
	void InitViewport(D3D11_VIEWPORT & _viewport, RECT clientSize);
	void InitDeviceContextSwapchain(RECT clientSize);
	void InitRenderTargetView(UEngine::RenderToTexture & rtt);
	void InitDepthStencil(UEngine::RenderToTexture & rtt, RECT clientSize,
		D3D11_TEXTURE2D_DESC depthBuffer, D3D11_DEPTH_STENCIL_DESC depthState,
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV);
	void InitConstBuffer(UINT byteWidth, ID3D11Buffer ** constBuffer);
	void InitInputLayout(UEngine::pipeline_state_t & pipeline, const void * pVShaderByteCode, SIZE_T VShaderLength,
		const void * pPShaderByteCode, SIZE_T PShaderLength, const void * pGShaderByteCode,
		SIZE_T GShaderLength, const D3D11_INPUT_ELEMENT_DESC * vLayout, UINT layoutLength);

	void CreateRenderToTexture(UEngine::RenderToTexture * rtt, UINT width, UINT height);
	void RequestNewRTT(UEngine::RenderToTexture * rtt, UINT width, UINT height, ID3D11DeviceContext ** m_pWorldDeferredContext);

	// add more pipelines when it is necessary
	void AddBasicPipelines();
};

