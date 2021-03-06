#include "stdafx.h"
#include "Renderer.h"


Renderer::Renderer()
{
	loadingDone = false;
	constBufferWorld = nullptr;
	constBufferScene = nullptr;
	constBufferDLight = nullptr;
	constBufferPLight = nullptr;
	constBufferSLight = nullptr;
	constBufferLightInfo = nullptr;
	constBufferRTTPos = nullptr;
	constBufferRTTSize = nullptr;
	constBufferParticleWorld = nullptr;
}
Renderer::~Renderer()
{
	if (constBufferWorld)
		constBufferWorld->Release();
	if (constBufferScene)
		constBufferScene->Release();
	if (constBufferDLight)
		constBufferDLight->Release();
	if (constBufferPLight)
		constBufferPLight->Release();
	if (constBufferSLight)
		constBufferSLight->Release();
	if (constBufferLightInfo)
		constBufferLightInfo->Release();
	if (constBufferRTTPos)
		constBufferRTTPos->Release();
	if (constBufferRTTSize)
		constBufferRTTSize->Release();
	if (constBufferParticleWorld)
		constBufferParticleWorld->Release();
	// Clean up
	if (computeShader)
		computeShader->Release();
}

void Renderer::AddCameras(CameraComponent * component, RECT clientSize)
{
	D3D11_VIEWPORT * rttViewPort = new D3D11_VIEWPORT;
	InitViewport(*rttViewPort, clientSize);
	component->SetViewport(rttViewPort);

#pragma region WORLD_RTT
	UEngine::RenderToTexture * rtt = new UEngine::RenderToTexture;
	CreateRenderToTexture(rtt, (UINT)rttViewPort->Width, (UINT)rttViewPort->Height);
	component->SetRTTWorld(rtt);
#pragma endregion
#pragma region UI_RTT
	rtt = new UEngine::RenderToTexture;
	CreateRenderToTexture(rtt, (UINT)rttViewPort->Width, (UINT)rttViewPort->Height);
	component->SetRTTUI(rtt);
#pragma endregion
	component->CreateNewDeferredContext(m_pDevice.Get());

	m_pCameras.push_back(component);
}

void Renderer::Init()
{
	RECT clientSize;
	GetClientRect(hWnd, &clientSize);
	InitViewport(default_viewport, clientSize);
	InitDeviceContextSwapchain(clientSize);
#pragma region INITIALIZE
	std::vector<std::thread> threads;
	threads.push_back(std::thread([&]() {
		InitRenderTargetView(default_RTT);
	}));	// render target view
	threads.push_back(std::thread([&]() {
#pragma region descDepth_buffer
		D3D11_TEXTURE2D_DESC depthBuffer;
		depthBuffer.Width = (UINT)(clientSize.right - clientSize.left);
		depthBuffer.Height = (UINT)(clientSize.bottom - clientSize.top);
		depthBuffer.MipLevels = 1;
		depthBuffer.ArraySize = 1;
		depthBuffer.Format = DXGI_FORMAT_D32_FLOAT;
		depthBuffer.SampleDesc.Count = m_pSwapchainDesc.SampleDesc.Count;
		depthBuffer.SampleDesc.Quality = m_pSwapchainDesc.SampleDesc.Quality;
		depthBuffer.Usage = D3D11_USAGE_DEFAULT;
		depthBuffer.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBuffer.CPUAccessFlags = NULL;
		depthBuffer.MiscFlags = NULL;
#pragma endregion
#pragma region depth_stencil_state_desc
		D3D11_DEPTH_STENCIL_DESC depthState;

		// Depth test parameters
		depthState.DepthEnable = true;
		depthState.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthState.DepthFunc = D3D11_COMPARISON_LESS;

		// Stencil test parameters
		depthState.StencilEnable = true;
		depthState.StencilReadMask = 0xFF;
		depthState.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing
		depthState.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthState.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthState.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthState.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing
		depthState.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthState.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthState.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthState.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
#pragma endregion
#pragma region depth_stencil_view_desc
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		descDSV.Format = DXGI_FORMAT_D32_FLOAT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		descDSV.Flags = NULL;
		descDSV.Texture2D.MipSlice = NULL;
#pragma endregion
		InitDepthStencil(default_RTT, clientSize, depthBuffer, depthState, descDSV);
	}));	// depth stencil buffer/state/view
	threads.push_back(std::thread([&]() {
		if (m_pDevice)
		{
			D3D11_RASTERIZER_DESC rasterizerState;
			rasterizerState.FillMode = D3D11_FILL_SOLID;
			rasterizerState.CullMode = D3D11_CULL_NONE;
			rasterizerState.FrontCounterClockwise = false;
			rasterizerState.DepthBias = false;
			rasterizerState.DepthBiasClamp = 0;
			rasterizerState.SlopeScaledDepthBias = 0;
			rasterizerState.DepthClipEnable = true;
			rasterizerState.ScissorEnable = false;
			rasterizerState.MultisampleEnable = true;
			rasterizerState.AntialiasedLineEnable = true;
			m_pDevice->CreateRasterizerState(&rasterizerState, default_pipeline.rasterState.GetAddressOf());
		}
	}));	// rasterizer state
	threads.push_back(std::thread([&]() {
		// Create view layout
		D3D11_INPUT_ELEMENT_DESC vLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		InitInputLayout(default_pipeline, 
			Default_VS, ARRAYSIZE(Default_VS), 
			Default_PS, ARRAYSIZE(Default_PS), 
			Default_GS, ARRAYSIZE(Default_GS), 
			vLayout, ARRAYSIZE(vLayout));
	}));	// inputlayout
	threads.push_back(std::thread([&]() {
		if (m_pDevice)
		{
			D3D11_SAMPLER_DESC samplerDesc;
			ZeroMemory(&samplerDesc, sizeof(samplerDesc));
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			samplerDesc.MaxAnisotropy = 1;
			samplerDesc.MaxLOD = FLT_MAX;
			samplerDesc.MinLOD = -FLT_MAX;
			samplerDesc.MipLODBias = 0;
			m_pDevice->CreateSamplerState(&samplerDesc, &default_pipeline.samplerState);
		}
	})); // create sampler (filtering)
	threads.push_back(std::thread([&]() {
		if (m_pDevice)
		{
			D3D11_BLEND_DESC omDesc;
			ZeroMemory(&omDesc, sizeof(D3D11_BLEND_DESC));
			omDesc.AlphaToCoverageEnable = true;
			omDesc.IndependentBlendEnable = false;
			omDesc.RenderTarget[0].BlendEnable = true;
			omDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			omDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
			omDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			omDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			omDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			omDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			omDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			m_pDevice->CreateBlendState(&omDesc, &default_pipeline.blendingState);
		}
	})); // Create Blending
#pragma endregion
	threads.push_back(std::thread([&]() {
		InitConstBuffer(sizeof(DirectX::XMMATRIX), &constBufferWorld);
		InitConstBuffer(sizeof(SCENE), &constBufferScene);
		InitConstBuffer(sizeof(DLIGHT), &constBufferDLight);
		InitConstBuffer(sizeof(SLIGHT), &constBufferSLight);
		InitConstBuffer(sizeof(PLIGHT), &constBufferPLight);
		InitConstBuffer(sizeof(LIGHTINFO), &constBufferLightInfo);
		InitConstBuffer(sizeof(DirectX::XMFLOAT4), &constBufferRTTPos);
		InitConstBuffer(sizeof(DirectX::XMFLOAT4), &constBufferRTTSize);
		InitConstBuffer(sizeof(UEngine::ParticleConstBuffer) * 1000, &constBufferParticleWorld);
	}));	// TODO: add constant buffers
	threads.push_back(std::thread([&]() {
		AddBasicPipelines();
	})); // TODO: Adding pipelines
	for (auto& thread : threads) {
		thread.join();
	}
	threads.clear();

	if (m_pDevice)
	{
		// Verify compute shader is supported
		if (m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0)
		{
			D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS hwopts = { 0 };
			(void)m_pDevice->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &hwopts, sizeof(hwopts));
			if (!hwopts.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x)
			{
				m_pDevice.ReleaseAndGetAddressOf();
				printf("DirectCompute is not supported by this device\n");
			}
		}
		// Compile shader
		ID3DBlob *csBlob = nullptr;
		HRESULT hr = CompileComputeShader(L"Shaders/Particle_CS.hlsl", "main", m_pDevice.Get(), &csBlob);
		if (FAILED(hr))
		{
			m_pDevice.ReleaseAndGetAddressOf();
			printf("Failed compiling shader %08X\n", hr);
		}
		// Create shader
		hr = m_pDevice->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), nullptr, &computeShader);

		csBlob->Release();

		if (FAILED(hr))
		{
			m_pDevice.ReleaseAndGetAddressOf();
		}

		printf("Success\n");
		m_pDeviceContext->CSSetShader(computeShader, nullptr, 0);


#ifdef _DEBUG
		// debugrenderer
		debugRenderer.Init(m_pDevice.Get(), m_pDeviceContext.Get());
#endif

#pragma region Default_Vertex
		// Basic Model Loading
		UEngine::DefaultVertex cpu_vertex;
		cpu_vertex.pos = DirectX::XMFLOAT3();
		cpu_vertex.tex = DirectX::XMFLOAT2();

		// create all buffers
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.ByteWidth = (UINT)sizeof(UEngine::DefaultVertex);
		D3D11_SUBRESOURCE_DATA initData;
		ZeroMemory(&initData, sizeof(initData));
		initData.pSysMem = &cpu_vertex;
		m_pDevice->CreateBuffer(&bufferDesc, &initData, default_vertexBuffer.GetAddressOf());
#pragma endregion
	}

	loadingDone = true;
}
void Renderer::Update(ObjectManager * objManager)
{
	if (!loadingDone)
		return;

	// render for each camera
	UVector<int> threads;
	for (unsigned i = 0; i < m_pCameras.size(); i++)
	{
		if (m_pCameras[i]->GetActive() && m_pCameras[i]->GetParent()->GetActive())
			threads.push_back(threadPool.AddTask(std::bind(&Renderer::DeferredRendering, this, objManager, i)));
	}
	for (unsigned i = 0; i < threads.size(); i++)
	{
		threadPool.Join(threads[i]);
	}
	threads.clear();

#ifdef _DEBUG
	// render verts in debug renderer
	CameraComponent * debugCamera = m_pCameras[0];
	debugCamera->GetDeferredContext(UEngine::DrawType_WORLD)->OMSetDepthStencilState(debugCamera->GetRTTWorld()->depthStencilState.Get(), 1);
	debugCamera->GetDeferredContext(UEngine::DrawType_WORLD)->OMSetRenderTargets(1, debugCamera->GetRTTWorld()->renderTargetViewMap.GetAddressOf(), debugCamera->GetRTTWorld()->depthStencilView.Get());
	debugCamera->GetDeferredContext(UEngine::DrawType_WORLD)->ClearDepthStencilView(debugCamera->GetRTTWorld()->depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	DebugSet(&m_pPipelines[UEngine::PipelineType_DebugRender], debugCamera);
	debugRenderer.Flush(debugCamera->GetDeferredContext(UEngine::DrawType_WORLD));
#endif

	// finish deferred rendering
	for (unsigned i = 0; i < m_pCameras.size(); i++)
	{
		if (m_pCameras[i]->GetActive() && m_pCameras[i]->GetParent()->GetActive())
			threads.push_back(threadPool.AddTask(std::bind(&Renderer::FinishComands, this, i)));
	}
	for (unsigned i = 0; i < threads.size(); i++)
	{
		threadPool.Join(threads[i]);
	}
	threads.clear();
	// execute deferred rendering
	for (unsigned i = 0; i < m_pCameras.size(); i++)
	{
		if (!m_pCameras[i]->GetActive() || !m_pCameras[i]->GetParent()->GetActive())
			continue;

		if (*m_pCameras[i]->GetCommandList(0))
			m_pDeviceContext->ExecuteCommandList(*m_pCameras[i]->GetCommandList(0), true); // Execute pass 1.
		if (*m_pCameras[i]->GetCommandList(1))
			m_pDeviceContext->ExecuteCommandList(*m_pCameras[i]->GetCommandList(1), true); // Execute pass 1.
		m_pCameras[i]->ReleaseCommandList(0);
		m_pCameras[i]->ReleaseCommandList(1);
	}

	UINT stride = sizeof(UEngine::DefaultVertex);
	UINT offset = 0;

	// clearing depth buffer and render target
	m_pDeviceContext->OMSetDepthStencilState(default_RTT.depthStencilState.Get(), 1);
	m_pDeviceContext->OMSetRenderTargets(1, default_RTT.renderTargetViewMap.GetAddressOf(), default_RTT.depthStencilView.Get());

	m_pDeviceContext->ClearRenderTargetView(default_RTT.renderTargetViewMap.Get(), DirectX::Colors::Transparent);
	RenderSet(m_pDeviceContext.Get(), default_pipeline, default_RTT, default_viewport, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	for (unsigned i = 0; i < m_pCameras.size(); i++)
	{
		if (!m_pCameras[i]->GetActive() || !m_pCameras[i]->GetParent()->GetActive())
			continue;

		m_pDeviceContext->VSSetConstantBuffers(0, 1, &constBufferRTTPos);
		DirectX::XMFLOAT2 viewCenter = DirectX::XMFLOAT2(m_pCameras[i]->GetViewport()->Width / 2.0f + m_pCameras[i]->GetViewport()->TopLeftX,
			m_pCameras[i]->GetViewport()->Height / 2.0f + m_pCameras[i]->GetViewport()->TopLeftY);
		viewCenter.x /= (default_viewport.Width / 2.0f + default_viewport.TopLeftX);
		viewCenter.x = viewCenter.x - 1.0f;
		viewCenter.y /= (default_viewport.Height / 2.0f + default_viewport.TopLeftY);
		viewCenter.y = -viewCenter.y + 1.0f;
		DirectX::XMFLOAT2 viewLength = DirectX::XMFLOAT2((m_pCameras[i]->GetViewRatio().z - m_pCameras[i]->GetViewRatio().x),
			(m_pCameras[i]->GetViewRatio().w - m_pCameras[i]->GetViewRatio().y));

		DirectX::XMFLOAT4 RTPos = DirectX::XMFLOAT4(viewCenter.x, viewCenter.y, 0, 1);
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		m_pDeviceContext->Map(constBufferRTTPos, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
		memcpy(mappedResource.pData, &RTPos, sizeof(DirectX::XMFLOAT4));
		m_pDeviceContext->Unmap(constBufferRTTPos, 0);

		m_pDeviceContext->GSSetConstantBuffers(1, 1, &constBufferRTTSize);
		DirectX::XMFLOAT4 RTSize = DirectX::XMFLOAT4(-viewLength.x, -viewLength.y, viewLength.x, viewLength.y);
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		m_pDeviceContext->Map(constBufferRTTSize, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
		memcpy(mappedResource.pData, &RTSize, sizeof(DirectX::XMFLOAT4));
		m_pDeviceContext->Unmap(constBufferRTTSize, 0);

		// set vertex info
		m_pDeviceContext->ClearDepthStencilView(default_RTT.depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
		m_pDeviceContext->IASetVertexBuffers(0, 1, default_vertexBuffer.GetAddressOf(), &stride, &offset);

		// convert msaa srv to single sampling srv
		m_pDeviceContext->ResolveSubresource((ID3D11Resource*)m_pCameras[i]->GetRTTWorld()->outTexture.Get(), D3D11CalcSubresource(0, 0, 1),
			(ID3D11Resource*)m_pCameras[i]->GetRTTWorld()->renderTargetTextureMap.Get(), D3D11CalcSubresource(0, 0, 1), DXGI_FORMAT_R32G32B32A32_FLOAT);
		ID3D11ShaderResourceView *baseTexture[]{
			(ID3D11ShaderResourceView*)
			m_pCameras[i]->GetRTTWorld()->outSRV.Get()
		};
		m_pDeviceContext->PSSetShaderResources(0, 1, baseTexture);

		// Draw
		m_pDeviceContext->Draw(1, 0);

		// clear depth for UI
		m_pDeviceContext->ClearDepthStencilView(default_RTT.depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

		// convert msaa srv to single sampling srv
		m_pDeviceContext->ResolveSubresource((ID3D11Resource*)m_pCameras[i]->GetRTTUI()->outTexture.Get(), D3D11CalcSubresource(0, 0, 1),
			(ID3D11Resource*)m_pCameras[i]->GetRTTUI()->renderTargetTextureMap.Get(), D3D11CalcSubresource(0, 0, 1), DXGI_FORMAT_R32G32B32A32_FLOAT);
		ID3D11ShaderResourceView *baseTextureUI[]{
			(ID3D11ShaderResourceView*)
			m_pCameras[i]->GetRTTUI()->outSRV.Get()
		};
		m_pDeviceContext->PSSetShaderResources(0, 1, baseTextureUI);

		// Draw
		m_pDeviceContext->Draw(1, 0);
	}

	m_pSwapCahin->Present(0, 0);
}

void Renderer::DeferredRendering(ObjectManager * objManager, int i) {
	if (!m_pCameras[i]->GetActive() || !m_pCameras[i]->GetParent()->GetActive())
		return;
	objManager->Render(m_pCameras[i], this);
}
void Renderer::FinishComands(int i) {
	if (!m_pCameras[i]->GetActive() || !m_pCameras[i]->GetParent()->GetActive())
		return;

	// Create command lists and record commands into them.
	int t0 = threadPool.AddTask([&]() {
		m_pCameras[i]->GetDeferredContext(0)->FinishCommandList(true, m_pCameras[i]->GetCommandList(0));
	});
	int t1 = threadPool.AddTask([&]() {
		m_pCameras[i]->GetDeferredContext(1)->FinishCommandList(true, m_pCameras[i]->GetCommandList(1));
	});
	
	threadPool.Join(t0);
	threadPool.Join(t1);
}

void Renderer::Resize(bool isFullScreen, int width, int height) {
	m_pSwapCahin->SetFullscreenState(false, nullptr);
	MoveWindow(hWnd, 0, 0, width, height, true);
	RECT clientSize;
	GetClientRect(hWnd, &clientSize);
	default_RTT.depthStencilBuffer.ReleaseAndGetAddressOf();
	default_RTT.depthStencilState.ReleaseAndGetAddressOf();
	default_RTT.depthStencilView.ReleaseAndGetAddressOf();
	default_RTT.renderTargetViewMap.ReleaseAndGetAddressOf();
	m_pDeviceContext->Flush();

	// swapchain specification
	m_pSwapchainDesc.BufferDesc.Width = (UINT)(clientSize.right - clientSize.left);
	m_pSwapchainDesc.BufferDesc.Height = (UINT)(clientSize.bottom - clientSize.top);
	m_pSwapCahin->ResizeBuffers(1, m_pSwapchainDesc.BufferDesc.Width, m_pSwapchainDesc.BufferDesc.Height,
		m_pSwapchainDesc.BufferDesc.Format, m_pSwapchainDesc.Flags);
	InitViewport(default_viewport, clientSize);

	std::vector<std::thread> threads;
	threads.push_back(std::thread([&]() {
		InitRenderTargetView(default_RTT);
	}));	// render target view
	threads.push_back(std::thread([&]() {
#pragma region descDepth_buffer
		D3D11_TEXTURE2D_DESC depthBuffer;
		depthBuffer.Width = (UINT)(clientSize.right - clientSize.left);
		depthBuffer.Height = (UINT)(clientSize.bottom - clientSize.top);
		depthBuffer.MipLevels = 1;
		depthBuffer.ArraySize = 1;
		depthBuffer.Format = DXGI_FORMAT_D32_FLOAT;
		depthBuffer.SampleDesc.Count = m_pSwapchainDesc.SampleDesc.Count;
		depthBuffer.SampleDesc.Quality = m_pSwapchainDesc.SampleDesc.Quality;
		depthBuffer.Usage = D3D11_USAGE_DEFAULT;
		depthBuffer.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBuffer.CPUAccessFlags = NULL;
		depthBuffer.MiscFlags = NULL;
#pragma endregion
#pragma region depth_stencil_state_desc
		D3D11_DEPTH_STENCIL_DESC depthState;

		// Depth test parameters
		depthState.DepthEnable = true;
		depthState.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthState.DepthFunc = D3D11_COMPARISON_LESS;

		// Stencil test parameters
		depthState.StencilEnable = true;
		depthState.StencilReadMask = 0xFF;
		depthState.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing
		depthState.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthState.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthState.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthState.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing
		depthState.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthState.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthState.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthState.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
#pragma endregion
#pragma region depth_stencil_view_desc
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		descDSV.Format = DXGI_FORMAT_D32_FLOAT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		descDSV.Flags = NULL;
		descDSV.Texture2D.MipSlice = NULL;
#pragma endregion
		InitDepthStencil(default_RTT, clientSize, depthBuffer, depthState, descDSV);
	}));	// depth stencil buffer/state/view
	for (auto& thread : threads) {
		thread.join();
	}
	threads.clear();

	for (unsigned i = 0; i < m_pCameras.size(); i++)
	{
		m_pCameras[i]->Clear();

		RECT changedSize;
		changedSize.left = (LONG)(clientSize.right * m_pCameras[i]->GetViewRatio().x);
		changedSize.top = (LONG)(clientSize.bottom * m_pCameras[i]->GetViewRatio().y);
		changedSize.right = (LONG)(clientSize.right * m_pCameras[i]->GetViewRatio().z);
		changedSize.bottom = (LONG)(clientSize.bottom *m_pCameras[i]->GetViewRatio().w);
		D3D11_VIEWPORT * rttViewPort = new D3D11_VIEWPORT;
		InitViewport(*rttViewPort, changedSize);
		m_pCameras[i]->SetViewport(rttViewPort);
#pragma region WORLD_RTT
		UEngine::RenderToTexture * rtt = new UEngine::RenderToTexture;
		CreateRenderToTexture(rtt, (UINT)rttViewPort->Width, (UINT)rttViewPort->Height);
		m_pCameras[i]->SetRTTWorld(rtt);
#pragma endregion
#pragma region UI_RTT
		rtt = new UEngine::RenderToTexture;
		CreateRenderToTexture(rtt, (UINT)rttViewPort->Width, (UINT)rttViewPort->Height);
		m_pCameras[i]->SetRTTUI(rtt);
#pragma endregion
	}
	m_pSwapCahin->SetFullscreenState(isFullScreen, nullptr);
}
void Renderer::LoadObject(const char * name, GameObject * gameObject) {
	if (m_pDevice)
	{
		RenderComponent * model = new Render_World();
		model->Init(&m_pPipelines[UEngine::PipelineType_NO_ANIMATION], gameObject);
		model->ReadBin(name, m_pDevice.Get(), m_pDeviceContext.Get());
		model->SetType(UEngine::DrawType_WORLD);
		gameObject->SetRenderComponent(model);
		gameObject->GetTransform()->SetMatrix(DirectX::XMMatrixIdentity());
	}
}
void Renderer::LoadGUI(const char * textureName, GameObject * gameObject, unsigned cameraIndex) {
	if (m_pDevice)
	{
		// load texture
		RenderComponent * textModel = new Render_UI();
		Render_UI * ptr = (Render_UI*)textModel;
		ptr->Init(&m_pPipelines[UEngine::PipelineType_UI], gameObject);
		ptr->ReadBin(textureName, m_pDevice.Get(), m_pDeviceContext.Get());
		ptr->SetType(UEngine::DrawType_UI);
		ptr->SetCamera(cameraIndex);
		gameObject->SetRenderComponent(textModel);
		gameObject->GetTransform()->SetMatrix(DirectX::XMMatrixIdentity());
	}
}
void Renderer::LoadGUI(const WCHAR * inputString, unsigned length, GameObject * gameObject, unsigned cameraIndex, UEngine::TextFormat textFormat) {
	if (m_pDevice)
	{
		// load texture
		RenderComponent * textModel = new Render_UI();
		Render_UI * ptr = (Render_UI*)textModel;
		ptr->Init(&m_pPipelines[UEngine::PipelineType_UI], gameObject);
		ptr->Init(m_pDevice.Get(), inputString, length, L"Verdana", 30, textFormat);
		ptr->SetType(UEngine::DrawType_UI);
		ptr->SetCamera(cameraIndex);
		gameObject->SetRenderComponent(textModel);
		gameObject->GetTransform()->SetMatrix(DirectX::XMMatrixIdentity());
	}
}
void Renderer::LoadParticle(const char * name, GameObject * gameObject) {
	if (m_pDevice)
	{
		RenderComponent * particleModel = new Render_Particle();
		Render_Particle * ptr = (Render_Particle*)particleModel;
		ptr->Init(&m_pPipelines[UEngine::PipelineType_PARTICLE], gameObject);
		ptr->Init(m_pDevice.Get());
		ptr->ReadBin(name, m_pDevice.Get(), m_pDeviceContext.Get());
		ptr->SetType(UEngine::DrawType_WORLD);
		gameObject->SetRenderComponent(particleModel);
		gameObject->GetTransform()->SetMatrix(DirectX::XMMatrixIdentity());
	}
}
void Renderer::ChangeGUI(const char * textStr, GameObject * gameObject, UEngine::TextFormat * textFormat) {
	size_t newsize = strlen(textStr) + 1;
	wchar_t * wcstring = new wchar_t[newsize];
	// Convert char* string to a wchar_t* string.  
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, wcstring, newsize, textStr, _TRUNCATE);
	if (textFormat)
		((Render_UI*)gameObject->GetRenderComponent())->SetTextFormat(*textFormat);
	((Render_UI*)gameObject->GetRenderComponent())->ChangeText(m_pDevice.Get(), m_pDeviceContext.Get(), wcstring, (UINT32)newsize);
	delete[] wcstring;
}

void Renderer::RenderSet(ID3D11DeviceContext * m_pDeviceContext, UEngine::pipeline_state_t & pipeline, UEngine::RenderToTexture & rtt,
	D3D11_VIEWPORT & viewport, D3D11_PRIMITIVE_TOPOLOGY topology) {
	// clearing backbuffer
	m_pDeviceContext->OMSetDepthStencilState(rtt.depthStencilState.Get(), 1);
	m_pDeviceContext->OMSetRenderTargets(1, rtt.renderTargetViewMap.GetAddressOf(), rtt.depthStencilView.Get());
	m_pDeviceContext->OMSetBlendState(pipeline.blendingState.Get(), NULL, 0xffffffff);
	m_pDeviceContext->RSSetViewports(1, &viewport);
	
	// Bind depth stencil state
	m_pDeviceContext->RSSetState(pipeline.rasterState.Get());
	ID3D11SamplerState *sampler[]{ pipeline.samplerState.Get() };
	m_pDeviceContext->PSSetSamplers(0, 1, sampler);

	m_pDeviceContext->VSSetShader(pipeline.vertex_shader.Get(), nullptr, 0);
	m_pDeviceContext->PSSetShader(pipeline.pixel_shader.Get(), nullptr, 0);
	m_pDeviceContext->GSSetShader(pipeline.geometry_shader.Get(), nullptr, 0);
	m_pDeviceContext->IASetInputLayout(pipeline.input_layout.Get());
	m_pDeviceContext->IASetPrimitiveTopology(topology);
}
void Renderer::DebugSet(UEngine::pipeline_state_t * pipeline, Component * m_pCamera) {
	CameraComponent * camera = (CameraComponent*)m_pCamera;
	camera->GetDeferredContext(UEngine::DrawType_WORLD)->OMSetBlendState(pipeline->blendingState.Get(), NULL, 0xffffffff);
	camera->GetDeferredContext(UEngine::DrawType_WORLD)->RSSetState(pipeline->rasterState.Get());
	ID3D11SamplerState *sampler[]{ pipeline->samplerState.Get() };
	camera->GetDeferredContext(UEngine::DrawType_WORLD)->PSSetSamplers(0, 1, sampler);

	// transparent stuff
	DirectX::XMMATRIX originalView = camera->GetOriginalView();
	DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant(originalView);
	float aspectRatio = ((camera->GetViewport()->Width) / (camera->GetViewport()->Height));
	camera->SetAspectRatio(aspectRatio);

	SCENE sceneToShader;
	sceneToShader.viewMat = DirectX::XMMatrixInverse(&determinant, originalView);
	sceneToShader.viewMat = DirectX::XMMatrixTranspose(sceneToShader.viewMat);
	sceneToShader.perspectivMat = DirectX::XMMatrixPerspectiveFovLH(camera->GetAngle(), aspectRatio, camera->GetNearZ(), DEBUG_RENDER_FAR_VIEW);
	sceneToShader.perspectivMat = DirectX::XMMatrixTranspose(sceneToShader.perspectivMat);

	// view matrix buffer
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	camera->GetDeferredContext(UEngine::DrawType_WORLD)->Map(constBufferScene, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
	memcpy(mappedResource.pData, &sceneToShader, sizeof(SCENE));
	camera->GetDeferredContext(UEngine::DrawType_WORLD)->Unmap(constBufferScene, 0);

	camera->GetDeferredContext(UEngine::DrawType_WORLD)->VSSetConstantBuffers(0, 1, &constBufferScene);

	camera->GetDeferredContext(UEngine::DrawType_WORLD)->VSSetShader(pipeline->vertex_shader.Get(), nullptr, 0);
	camera->GetDeferredContext(UEngine::DrawType_WORLD)->PSSetShader(pipeline->pixel_shader.Get(), nullptr, 0);
	camera->GetDeferredContext(UEngine::DrawType_WORLD)->GSSetShader(pipeline->geometry_shader.Get(), nullptr, 0);
	camera->GetDeferredContext(UEngine::DrawType_WORLD)->IASetInputLayout(pipeline->input_layout.Get());
	camera->GetDeferredContext(UEngine::DrawType_WORLD)->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

void Renderer::InitViewport(D3D11_VIEWPORT & _viewport, RECT clientSize)
{
	// viewport setting
	_viewport.Width = (float)(clientSize.right - clientSize.left);
	_viewport.Height = (float)(clientSize.bottom - clientSize.top);
	_viewport.MinDepth = 0;
	_viewport.MaxDepth = 1;
	_viewport.TopLeftX = (float)clientSize.left;
	_viewport.TopLeftY = (float)clientSize.top;
}
void Renderer::InitDeviceContextSwapchain(RECT clientSize) {
	// directX compatibility
	D3D_FEATURE_LEVEL FeatureLevels[2];
	FeatureLevels[0] = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;
	FeatureLevels[1] = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_1;

	// debugging directX
	D3D_FEATURE_LEVEL FeatureLevel;
	UINT flag = D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	// window specification
	ZeroMemory(&m_pSwapchainDesc, sizeof(m_pSwapchainDesc));
	m_pSwapchainDesc.BufferCount = 1;
	m_pSwapchainDesc.BufferDesc.Width = (UINT)(clientSize.right - clientSize.left);
	m_pSwapchainDesc.BufferDesc.Height = (UINT)(clientSize.bottom - clientSize.top);
	m_pSwapchainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_pSwapchainDesc.BufferDesc.RefreshRate.Numerator = 60;
	m_pSwapchainDesc.BufferDesc.RefreshRate.Denominator = 1;
	m_pSwapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	m_pSwapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	m_pSwapchainDesc.OutputWindow = hWnd;
	m_pSwapchainDesc.SampleDesc.Count = 4;
	m_pSwapchainDesc.SampleDesc.Quality = 0;
	m_pSwapchainDesc.Windowed = TRUE;

#ifdef _DEBUG
	flag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// create swapchain, device, and context
	D3D11CreateDeviceAndSwapChain(nullptr,
		D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		flag,
		FeatureLevels,
		2,
		D3D11_SDK_VERSION,
		&m_pSwapchainDesc,
		&m_pSwapCahin,
		&m_pDevice,
		&FeatureLevel,
		&m_pDeviceContext);
}
void Renderer::InitRenderTargetView(UEngine::RenderToTexture & rtt) {
	Microsoft::WRL::ComPtr<ID3D11Debug> pDebugger;
	if (m_pDevice)
	{
		m_pDevice->QueryInterface(__uuidof(ID3D11Debug), (void **)&pDebugger);
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pSwapChainBuffer;
		m_pSwapCahin->GetBuffer(0, __uuidof(pSwapChainBuffer), (void **)&pSwapChainBuffer);
		m_pDevice->CreateRenderTargetView(pSwapChainBuffer.Get(), nullptr, &rtt.renderTargetViewMap);
	}
}
void Renderer::InitDepthStencil(UEngine::RenderToTexture & rtt, RECT clientSize,
	D3D11_TEXTURE2D_DESC depthBuffer, D3D11_DEPTH_STENCIL_DESC depthState,
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV) {
	if (m_pDevice)
	{
		m_pDevice->CreateTexture2D(&depthBuffer, nullptr, rtt.depthStencilBuffer.GetAddressOf());
		// Create depth stencil state
		m_pDevice->CreateDepthStencilState(&depthState, rtt.depthStencilState.GetAddressOf());
		// Create the depth stencil view
		m_pDevice->CreateDepthStencilView(rtt.depthStencilBuffer.Get(), // Depth stencil texture
			&descDSV, // Depth stencil desc
			rtt.depthStencilView.GetAddressOf());  // [out] Depth stencil view
	}
}
void Renderer::InitConstBuffer(UINT byteWidth, ID3D11Buffer ** constBuffer)
{
	if (m_pDevice)
	{
		// Create Constant Buffers
		D3D11_BUFFER_DESC constBufferDesc;
		ZeroMemory(&constBufferDesc, sizeof(constBufferDesc));
		constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constBufferDesc.ByteWidth = byteWidth;
		m_pDevice->CreateBuffer(&constBufferDesc, nullptr, constBuffer);
	}
}
void Renderer::InitInputLayout(UEngine::pipeline_state_t & pipeline, const void * pVShaderByteCode, SIZE_T VShaderLength,
	const void * pPShaderByteCode, SIZE_T PShaderLength,
	const void * pGShaderByteCode, SIZE_T GShaderLength,
	const D3D11_INPUT_ELEMENT_DESC * vLayout, UINT layoutLength) {
	if (m_pDevice)
	{
		// vertex shader
		if (pVShaderByteCode)
			m_pDevice->CreateVertexShader(pVShaderByteCode, VShaderLength, nullptr, pipeline.vertex_shader.GetAddressOf());
		else
			pipeline.vertex_shader = nullptr;
		// pixel shader
		if (pPShaderByteCode)
			m_pDevice->CreatePixelShader(pPShaderByteCode, PShaderLength, nullptr, pipeline.pixel_shader.GetAddressOf());
		else
			pipeline.pixel_shader = nullptr;
		// geometry shader
		if (pGShaderByteCode)
			m_pDevice->CreateGeometryShader(pGShaderByteCode, GShaderLength, nullptr, pipeline.geometry_shader.GetAddressOf());
		else
			pipeline.geometry_shader = nullptr;

		// Create Input Layout
		if (pVShaderByteCode)
			m_pDevice->CreateInputLayout(vLayout, layoutLength, pVShaderByteCode, VShaderLength, pipeline.input_layout.GetAddressOf());
		else
			pipeline.input_layout = nullptr;
	}
}

void Renderer::CreateRenderToTexture(UEngine::RenderToTexture * rtt, UINT width, UINT height) {
	if (m_pDevice)
	{
#pragma region descDepth_buffer
		D3D11_TEXTURE2D_DESC depthBuffer;
		depthBuffer.Width = width;
		depthBuffer.Height = height;
		depthBuffer.MipLevels = 1;
		depthBuffer.ArraySize = 1;
		depthBuffer.Format = DXGI_FORMAT_D32_FLOAT;
		depthBuffer.SampleDesc.Count = m_pSwapchainDesc.SampleDesc.Count;
		depthBuffer.SampleDesc.Quality = m_pSwapchainDesc.SampleDesc.Quality;
		depthBuffer.Usage = D3D11_USAGE_DEFAULT;
		depthBuffer.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBuffer.CPUAccessFlags = NULL;
		depthBuffer.MiscFlags = NULL;
#pragma endregion
#pragma region depth_stencil_state_desc
		D3D11_DEPTH_STENCIL_DESC depthState;

		// Depth test parameters
		depthState.DepthEnable = true;
		depthState.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthState.DepthFunc = D3D11_COMPARISON_LESS;

		// Stencil test parameters
		depthState.StencilEnable = true;
		depthState.StencilReadMask = 0xFF;
		depthState.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing
		depthState.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthState.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthState.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthState.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing
		depthState.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthState.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthState.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthState.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
#pragma endregion
#pragma region depth_stencil_view_desc
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		descDSV.Format = DXGI_FORMAT_D32_FLOAT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		descDSV.Flags = NULL;
		descDSV.Texture2D.MipSlice = NULL;
#pragma endregion
		m_pDevice->CreateTexture2D(&depthBuffer, nullptr, rtt->depthStencilBuffer.GetAddressOf());
		// Create depth stencil state
		m_pDevice->CreateDepthStencilState(&depthState, rtt->depthStencilState.GetAddressOf());
		// Create the depth stencil view
		m_pDevice->CreateDepthStencilView(rtt->depthStencilBuffer.Get(), // Depth stencil texture
			&descDSV, // Depth stencil desc
			rtt->depthStencilView.GetAddressOf());  // [out] Depth stencil view

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.SampleDesc.Count = m_pSwapchainDesc.SampleDesc.Count;
		desc.SampleDesc.Quality = m_pSwapchainDesc.SampleDesc.Quality;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		m_pDevice->CreateTexture2D(&desc, nullptr, &rtt->renderTargetTextureMap);

		/////////////////////// Map's Render Target
		// Setup the description of the render target view.
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		renderTargetViewDesc.Format = desc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		HRESULT h_ok = m_pDevice->CreateRenderTargetView(rtt->renderTargetTextureMap.Get(), &renderTargetViewDesc, &rtt->renderTargetViewMap);

		rtt->width = width;
		rtt->height = height;

		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		// Setup the description of the shader resource view.
		D3D11_SHADER_RESOURCE_VIEW_DESC singleSRVDesc;
		singleSRVDesc.Format = desc.Format;
		singleSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		singleSRVDesc.Texture2D.MostDetailedMip = 0;
		singleSRVDesc.Texture2D.MipLevels = 1;

		m_pDevice->CreateTexture2D(&desc, nullptr, &rtt->outTexture);
		m_pDevice->CreateShaderResourceView(rtt->outTexture.Get(), &singleSRVDesc, &rtt->outSRV);
	}
}
void Renderer::RequestNewRTT(UEngine::RenderToTexture * rtt, UINT width, UINT height, ID3D11DeviceContext ** m_pWorldDeferredContext)
{
	CreateRenderToTexture(rtt, width, height);
	m_pDevice->CreateDeferredContext(NULL, m_pWorldDeferredContext);
}

void Renderer::AddBasicPipelines() {
	if (m_pDevice)
	{
		m_pPipelines.reserve(UEngine::PipelineType_COUNT);
#pragma region INITIALIZATION
		UEngine::pipeline_state_t pipeline;
		D3D11_RASTERIZER_DESC rasterizerState;
		rasterizerState.FillMode = D3D11_FILL_SOLID;
		rasterizerState.CullMode = D3D11_CULL_FRONT;
		rasterizerState.FrontCounterClockwise = false;
		rasterizerState.DepthBias = false;
		rasterizerState.DepthBiasClamp = 0;
		rasterizerState.SlopeScaledDepthBias = 0;
		rasterizerState.DepthClipEnable = true;
		rasterizerState.ScissorEnable = false;
		rasterizerState.MultisampleEnable = true;
		rasterizerState.AntialiasedLineEnable = true;

		// Create view layout
		D3D11_INPUT_ELEMENT_DESC vLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		InitInputLayout(pipeline, Static_VS, ARRAYSIZE(Static_VS), Static_PS, ARRAYSIZE(Static_PS), nullptr, 0, vLayout, ARRAYSIZE(vLayout));
#pragma endregion
		// PipelineType_NO_ANIMATION
		m_pDevice->CreateRasterizerState(&rasterizerState, pipeline.rasterState.GetAddressOf());
		pipeline.samplerState = default_pipeline.samplerState.Get();
		D3D11_BLEND_DESC omDesc;
		ZeroMemory(&omDesc, sizeof(D3D11_BLEND_DESC));
		omDesc.AlphaToCoverageEnable = true;
		omDesc.IndependentBlendEnable = false;
		omDesc.RenderTarget[0].BlendEnable = true;
		omDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		omDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		omDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		omDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		omDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		omDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		omDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		m_pDevice->CreateBlendState(&omDesc, &pipeline.blendingState);
		pipeline.drawType = UEngine::DrawType_WORLD;
		m_pPipelines[UEngine::PipelineType_NO_ANIMATION] = pipeline;

		// PipelineType_ANIMATION
		//m_pDevice->CreateVertexShader(pVShaderByteCode, VShaderLength, nullptr, pipeline.vertex_shader.GetAddressOf());
		pipeline.vertex_shader = nullptr;
		m_pPipelines[UEngine::PipelineType_ANIMATION] = pipeline;

		// PipelineType_Particle
		UEngine::pipeline_state_t particlePipeline;
		D3D11_INPUT_ELEMENT_DESC vLayoutParticle[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		InitInputLayout(particlePipeline,
			Particle_VS, ARRAYSIZE(Particle_VS),
			Particle_PS, ARRAYSIZE(Particle_PS),
			Particle_GS, ARRAYSIZE(Particle_GS),
			vLayoutParticle, ARRAYSIZE(vLayoutParticle));
		m_pDevice->CreateRasterizerState(&rasterizerState, particlePipeline.rasterState.GetAddressOf());
		D3D11_BLEND_DESC alphaBlending;
		ZeroMemory(&alphaBlending, sizeof(D3D11_BLEND_DESC));
		alphaBlending.AlphaToCoverageEnable = true;
		alphaBlending.RenderTarget[0].BlendEnable = true;
		alphaBlending.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		alphaBlending.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		alphaBlending.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		alphaBlending.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		alphaBlending.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		alphaBlending.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		alphaBlending.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		m_pDevice->CreateBlendState(&alphaBlending, &particlePipeline.blendingState);
		particlePipeline.samplerState = default_pipeline.samplerState.Get();
		particlePipeline.drawType = UEngine::DrawType_WORLD;
		m_pPipelines[UEngine::PipelineType_PARTICLE] = particlePipeline;

		// PipelineType_UI
		UEngine::pipeline_state_t uiPipeline;
		D3D11_INPUT_ELEMENT_DESC vLayoutUI[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		InitInputLayout(uiPipeline,
			Default_VS, ARRAYSIZE(Default_VS),
			Default_PS, ARRAYSIZE(Default_PS),
			Default_GS, ARRAYSIZE(Default_GS),
			vLayoutUI, ARRAYSIZE(vLayoutUI));
		uiPipeline.rasterState = default_pipeline.rasterState.Get();
		uiPipeline.samplerState = default_pipeline.samplerState.Get();
		uiPipeline.blendingState = default_pipeline.blendingState.Get();
		uiPipeline.drawType = UEngine::DrawType_UI;
		m_pPipelines[UEngine::PipelineType_UI] = uiPipeline;

#ifdef _DEBUG
		// PipelineType_DebugRender
		UEngine::pipeline_state_t debugRenderPipeline;
		D3D11_INPUT_ELEMENT_DESC vLayoutDebugRender[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		InitInputLayout(debugRenderPipeline,
			DebugRender_VS, ARRAYSIZE(DebugRender_VS),
			DebugRender_PS, ARRAYSIZE(DebugRender_PS),
			nullptr, 0,
			vLayoutDebugRender, ARRAYSIZE(vLayoutDebugRender));
		debugRenderPipeline.rasterState = default_pipeline.rasterState.Get();
		debugRenderPipeline.samplerState = default_pipeline.samplerState.Get();
		D3D11_BLEND_DESC additiveBlending;
		ZeroMemory(&additiveBlending, sizeof(D3D11_BLEND_DESC));
		additiveBlending.AlphaToCoverageEnable = true;
		additiveBlending.RenderTarget[0].BlendEnable = true;
		additiveBlending.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		additiveBlending.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		additiveBlending.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		additiveBlending.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		additiveBlending.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		additiveBlending.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		additiveBlending.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		m_pDevice->CreateBlendState(&additiveBlending, &debugRenderPipeline.blendingState);
		debugRenderPipeline.drawType = UEngine::DrawType_UI;
		m_pPipelines[UEngine::PipelineType_DebugRender] = debugRenderPipeline;
#endif
	}
}

HRESULT Renderer::CompileComputeShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint,
	_In_ ID3D11Device* device, _Outptr_ ID3DBlob** blob)
{
	if (!srcFile || !entryPoint || !device || !blob)
		return E_INVALIDARG;

	*blob = nullptr;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	// We generally prefer to use the higher CS shader profile when possible as CS 5.0 is better performance on 11-class hardware
	LPCSTR profile = (device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "cs_5_0" : "cs_4_0";

	const D3D_SHADER_MACRO defines[] =
	{
		NULL, NULL
	};

	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(srcFile, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint, profile,
		flags, 0, &shaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		if (shaderBlob)
			shaderBlob->Release();

		return hr;
	}

	*blob = shaderBlob;

	return hr;
}
