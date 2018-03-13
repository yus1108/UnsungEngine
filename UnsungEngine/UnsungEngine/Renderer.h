#pragma once
#include "RenderComponent.h"
#include "GameObject.h"
#include "ObjectManager.h"

class Renderer
{
	friend class Render_World;
	friend class Render_UI;
private:
	UEngine::pipeline_state_t default_pipeline;
	D3D11_VIEWPORT default_viewport;

	DXGI_SWAP_CHAIN_DESC									m_pSwapchainDesc;
	Microsoft::WRL::ComPtr<IDXGISwapChain>					m_pSwapCahin;
	Microsoft::WRL::ComPtr<ID3D11Device>					m_pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>				m_pDeviceContext;
	UVector<Microsoft::WRL::ComPtr<ID3D11DeviceContext>>	m_pWorldDeferredContext;
	UVector<Microsoft::WRL::ComPtr<ID3D11CommandList>>		m_pWorldCommandList;

	UVector<D3D11_VIEWPORT>		m_pViewports;
	UVector<UEngine::pipeline_state_t>	m_pPipelines;
	UVector<UEngine::RenderToTexture>	m_pRTT;

	Microsoft::WRL::ComPtr<ID3D11Buffer> default_vertexBuffer;

	ID3D11Buffer * constBufferWorld;
	ID3D11Buffer * constBufferScene;
	ID3D11Buffer * constBufferDLight;
	ID3D11Buffer * constBufferPLight;
	ID3D11Buffer * constBufferSLight;
	ID3D11Buffer * constBufferLightInfo;
	ID3D11Buffer * constBufferRTTPos;
	ID3D11Buffer * constBufferRTTSize;

	bool loadingDone;
public:
	Renderer();
	~Renderer();

	void Init();
	void Update(ObjectManager * objManager);

	void LoadObject(const char * name, GameObject * gameObject);
	void LoadGUI(const WCHAR * inputString, unsigned length, GameObject * gameObject);
	void ChangeGUI(const char * textStr, GameObject * gameObject, UEngine::TextFormat * textFormat = nullptr);

private:
	void RenderSet(ID3D11DeviceContext * m_pDeviceContext, UEngine::pipeline_state_t & pipeline,
		D3D11_VIEWPORT & viewport, D3D11_PRIMITIVE_TOPOLOGY topology);

	void InitViewport(D3D11_VIEWPORT & _viewport, RECT clientSize);
	void InitDeviceContextSwapchain(RECT clientSize);
	void InitRenderTargetView(UEngine::pipeline_state_t & pipeline);
	void InitDepthStencil(UEngine::pipeline_state_t & pipeline, RECT clientSize,
		D3D11_TEXTURE2D_DESC depthBuffer, D3D11_DEPTH_STENCIL_DESC depthState,
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV);
	void InitConstBuffer(UINT byteWidth, ID3D11Buffer ** constBuffer);
	void InitInputLayout(UEngine::pipeline_state_t & pipeline, const void * pVShaderByteCode, SIZE_T VShaderLength,
		const void * pPShaderByteCode, SIZE_T PShaderLength, const void * pGShaderByteCode,
		SIZE_T GShaderLength, const D3D11_INPUT_ELEMENT_DESC * vLayout, UINT layoutLength);

	void CreateNewDeferredContext(UVector<Microsoft::WRL::ComPtr<ID3D11DeviceContext>> & m_pDeferredContexts);
	void CreateRenderToTexture(UEngine::RenderToTexture & rtt, UINT width, UINT height);
	void AddNewLayer(RECT clientSize);
	void AddBasicPipelines();
	void RequestNewRTT(UEngine::RenderToTexture & rtt, UINT width, UINT height, ID3D11DeviceContext ** m_pWorldDeferredContext);
};

