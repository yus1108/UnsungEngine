#include "stdafx.h"
#include "Renderer.h"


Renderer::Renderer()
{
	loadingDone = false;
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
	})); // create sampler (filtering)
	threads.push_back(std::thread([&]() {
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
	}));	// TODO: add constant buffers
	threads.push_back(std::thread([&]() {
		AddNewLayer(clientSize);
		AddBasicPipelines();
	})); // Adding Render To Texture and pipelines
	for (auto& thread : threads) {
		thread.join();
	}
	threads.clear();

#pragma region MODEL_LOADING
	// Basic Model Loading
	DefaultVertex cpu_vertex;
	cpu_vertex.pos = DirectX::XMFLOAT3();
	cpu_vertex.tex = DirectX::XMFLOAT2();

	// create all buffers
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.ByteWidth = (UINT)sizeof(DefaultVertex);
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = &cpu_vertex;
	m_pDevice->CreateBuffer(&bufferDesc, &initData, default_vertexBuffer.GetAddressOf());
#pragma endregion

	loadingDone = true;
}
void Renderer::Update(ObjectManager * objManager)
{
	if (!loadingDone)
		return;

	for (unsigned i = 0; i < (unsigned)m_pRTT.size(); i++)
	{
		// clearing backbuffer
		m_pWorldDeferredContext[i]->OMSetDepthStencilState(m_pRTT[i].depthStencilState.Get(), 1);
		m_pWorldDeferredContext[i]->OMSetRenderTargets(1, m_pRTT[i].renderTargetViewMap.GetAddressOf(), m_pRTT[i].depthStencilView.Get());

		m_pWorldDeferredContext[i]->ClearRenderTargetView(m_pRTT[i].renderTargetViewMap.Get(), DirectX::Colors::Transparent);
		m_pWorldDeferredContext[i]->ClearDepthStencilView(m_pRTT[i].depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}
	objManager->Render(m_pRTT, m_pWorldDeferredContext, m_pWorldCommandList, this);
	for (unsigned int i = 0; i < m_pWorldCommandList.size(); i++)
	{
		m_pDeviceContext->ExecuteCommandList(m_pWorldCommandList[i].Get(), true); // Execute pass 1.
		m_pWorldCommandList[i].ReleaseAndGetAddressOf();
	}

	UINT stride = sizeof(DefaultVertex);
	UINT offset = 0;

	// clearing depth buffer and render target
	m_pDeviceContext->OMSetDepthStencilState(default_RTT.depthStencilState.Get(), 1);
	m_pDeviceContext->OMSetRenderTargets(1, default_RTT.renderTargetViewMap.GetAddressOf(), default_RTT.depthStencilView.Get());

	m_pDeviceContext->ClearRenderTargetView(default_RTT.renderTargetViewMap.Get(), DirectX::Colors::Transparent);
	m_pDeviceContext->ClearDepthStencilView(default_RTT.depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	RenderSet(m_pDeviceContext.Get(), default_pipeline, default_RTT, default_viewport, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	for (unsigned int i = 0; i < m_pRTT.size(); i++)
	{
		m_pDeviceContext->VSSetConstantBuffers(0, 1, &constBufferRTTPos);
		DirectX::XMFLOAT4 RTPos = DirectX::XMFLOAT4(0, 0, 0, 1);
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		m_pDeviceContext->Map(constBufferRTTPos, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
		memcpy(mappedResource.pData, &RTPos, sizeof(DirectX::XMFLOAT4));
		m_pDeviceContext->Unmap(constBufferRTTPos, 0);

		m_pDeviceContext->GSSetConstantBuffers(1, 1, &constBufferRTTSize);
		DirectX::XMFLOAT4 RTSize = DirectX::XMFLOAT4(-1, -1, 1, 1);
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		m_pDeviceContext->Map(constBufferRTTSize, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
		memcpy(mappedResource.pData, &RTSize, sizeof(DirectX::XMFLOAT4));
		m_pDeviceContext->Unmap(constBufferRTTSize, 0);

		// set vertex info
		m_pDeviceContext->ClearDepthStencilView(default_RTT.depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
		m_pDeviceContext->IASetVertexBuffers(0, 1, default_vertexBuffer.GetAddressOf(), &stride, &offset);

		// convert msaa srv to single sampling srv
		
		m_pDeviceContext->ResolveSubresource((ID3D11Resource*)m_pRTT[i].outTexture.Get(), D3D11CalcSubresource(0, 0, 1),
			(ID3D11Resource*)m_pRTT[i].renderTargetTextureMap.Get(), D3D11CalcSubresource(0, 0, 1), DXGI_FORMAT_R32G32B32A32_FLOAT);
		ID3D11ShaderResourceView *baseTexture[]{
			(ID3D11ShaderResourceView*)
			m_pRTT[i].outSRV.Get()
		};
		m_pDeviceContext->PSSetShaderResources(0, 1, baseTexture);

		// Set the index buffer.
		m_pDeviceContext->Draw(1, 0);
	}

	m_pSwapCahin->Present(0, 0);
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

	for (size_t i = 0; i < UEngine::DrawType_COUNT; i++)
	{
		m_pRTT[i].depthStencilBuffer.ReleaseAndGetAddressOf();
		m_pRTT[i].depthStencilState.ReleaseAndGetAddressOf();
		m_pRTT[i].depthStencilView.ReleaseAndGetAddressOf();
		m_pRTT[i].outSRV.ReleaseAndGetAddressOf();
		m_pRTT[i].outTexture.ReleaseAndGetAddressOf();
		m_pRTT[i].renderTargetTextureMap.ReleaseAndGetAddressOf();
		m_pRTT[i].renderTargetViewMap.ReleaseAndGetAddressOf();
		m_pRTT[i].shaderResourceViewMap.ReleaseAndGetAddressOf();
		CreateRenderToTexture(m_pRTT[i], (UINT)(clientSize.right - clientSize.left), (UINT)(clientSize.bottom - clientSize.top));
		InitViewport(m_pViewports[i], clientSize);
	}
	m_pSwapCahin->SetFullscreenState(isFullScreen, nullptr);
}
void Renderer::LoadObject(const char * name, GameObject * gameObject) {
	RenderComponent * model = new Render_World();
	model->Init(m_pWorldDeferredContext[UEngine::DrawType_WORLD].Get(), &m_pPipelines[UEngine::PipelineType_NO_ANIMATION],
		&m_pRTT[UEngine::DrawType_WORLD], &m_pViewports[UEngine::DrawType_WORLD]);
	model->ReadBin(name, m_pDevice.Get(), m_pDeviceContext.Get());
	gameObject->SetRenderComponent(model);
	gameObject->SetDrawType(UEngine::DrawType_WORLD);
	gameObject->GetTransform()->SetMatrix(DirectX::XMMatrixIdentity());
}
void Renderer::LoadGUI(const char * textureName, GameObject * gameObject) {
	// load texture
	RenderComponent * textModel = new Render_UI();
	Render_UI * ptr = (Render_UI*)textModel;
	ptr->Init(m_pWorldDeferredContext[UEngine::DrawType_UI].Get(), &m_pPipelines[UEngine::PipelineType_UI],
		&m_pRTT[UEngine::DrawType_UI], &m_pViewports[UEngine::PipelineType_UI]);
	ptr->ReadBin(textureName, m_pDevice.Get(), m_pDeviceContext.Get());
	gameObject->SetRenderComponent(textModel);
	gameObject->SetDrawType(UEngine::DrawType_UI);
	gameObject->GetTransform()->SetMatrix(DirectX::XMMatrixIdentity());
}
void Renderer::LoadGUI(const WCHAR * inputString, unsigned length, GameObject * gameObject) {
	// load texture
	RenderComponent * textModel = new Render_UI();
	Render_UI * ptr = (Render_UI*)textModel;
	UEngine::TextFormat textFormat;
	textFormat.textColor = D2D1::ColorF::White;
	textFormat.dpiX = 150;
	textFormat.dpiY = 200;
	textModel->Init(m_pWorldDeferredContext[UEngine::DrawType_UI].Get(), &m_pPipelines[UEngine::PipelineType_UI], 
		&m_pRTT[UEngine::DrawType_UI], &m_pViewports[UEngine::DrawType_UI]);
	ptr->Init(m_pDevice.Get(), inputString, length, L"Verdana", 50, textFormat);
	gameObject->SetRenderComponent(textModel);
	gameObject->SetDrawType(UEngine::DrawType_UI);
	gameObject->GetTransform()->SetMatrix(DirectX::XMMatrixIdentity());
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
void Renderer::DebugSet(UEngine::pipeline_state_t * pipeline) {
	//ID3D11SamplerState *sampler[]{ pipeline->samplerState.Get() };
	//m_pDeviceContext->PSSetSamplers(0, 1, sampler);

	//// transparent stuff
	//DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant(myCam.GetOriginalView());
	//SCENE sceneToShader;
	//sceneToShader.viewMat = DirectX::XMMatrixInverse(&determinant, myCam.GetOriginalView());
	//sceneToShader.viewMat = DirectX::XMMatrixTranspose(sceneToShader.viewMat);
	//myCam.SetAspectRatio(default_viewport.Width / default_viewport.Height);
	//sceneToShader.perspectivMat = DirectX::XMMatrixPerspectiveFovLH(myCam.GetAngle(),
	//	myCam.GetAspectRatio(), myCam.nearZ, myCam.farZ);
	//sceneToShader.perspectivMat = DirectX::XMMatrixTranspose(sceneToShader.perspectivMat);
	//myCam.SetSceneToShader(sceneToShader);

	//// world matrix
	//D3D11_MAPPED_SUBRESOURCE mappedResource;
	//ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	//m_pDeviceContext->Map(constBufferWorld, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
	//memcpy(mappedResource.pData, &DirectX::XMMatrixIdentity(), sizeof(DirectX::XMMATRIX));
	//m_pDeviceContext->Unmap(constBufferWorld, 0);

	//// view matrix buffer
	//ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	//m_pDeviceContext->Map(constBufferScene, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
	//memcpy(mappedResource.pData, &myCam.GetSceneToShader(), sizeof(SCENE));
	//m_pDeviceContext->Unmap(constBufferScene, 0);

	//m_pDeviceContext->VSSetConstantBuffers(0, 1, &constBufferWorld);
	//m_pDeviceContext->VSSetConstantBuffers(1, 1, &constBufferScene);

	//m_pDeviceContext->VSSetShader(pipeline->vertex_shader[VertexShaderType::EColorVShader].Get(), nullptr, 0);
	//m_pDeviceContext->PSSetShader(pipeline->pixel_shader[PixelShaderType::EColorPShader].Get(), nullptr, 0);
	//m_pDeviceContext->IASetInputLayout(pipeline->input_layout.Get());
	//m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
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
	D3D_FEATURE_LEVEL FeatureLevels[4];
	FeatureLevels[0] = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_10_0;
	FeatureLevels[1] = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_10_1;
	FeatureLevels[2] = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;
	FeatureLevels[3] = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_1;

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
	m_pSwapchainDesc.SampleDesc.Count = 8;
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
		4,
		D3D11_SDK_VERSION,
		&m_pSwapchainDesc,
		&m_pSwapCahin,
		&m_pDevice,
		&FeatureLevel,
		&m_pDeviceContext);
}
void Renderer::InitRenderTargetView(UEngine::RenderToTexture & rtt) {
	Microsoft::WRL::ComPtr<ID3D11Debug> pDebugger;
	m_pDevice->QueryInterface(__uuidof(ID3D11Debug), (void **)&pDebugger);
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pSwapChainBuffer;
	m_pSwapCahin->GetBuffer(0, __uuidof(pSwapChainBuffer), (void **)&pSwapChainBuffer);
	m_pDevice->CreateRenderTargetView(pSwapChainBuffer.Get(), nullptr, &rtt.renderTargetViewMap);
}
void Renderer::InitDepthStencil(UEngine::RenderToTexture & rtt, RECT clientSize,
	D3D11_TEXTURE2D_DESC depthBuffer, D3D11_DEPTH_STENCIL_DESC depthState,
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV) {
	m_pDevice->CreateTexture2D(&depthBuffer, nullptr, rtt.depthStencilBuffer.GetAddressOf());
	// Create depth stencil state
	m_pDevice->CreateDepthStencilState(&depthState, rtt.depthStencilState.GetAddressOf());
	// Create the depth stencil view
	m_pDevice->CreateDepthStencilView(rtt.depthStencilBuffer.Get(), // Depth stencil texture
		&descDSV, // Depth stencil desc
		rtt.depthStencilView.GetAddressOf());  // [out] Depth stencil view
}
void Renderer::InitConstBuffer(UINT byteWidth, ID3D11Buffer ** constBuffer)
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
void Renderer::InitInputLayout(UEngine::pipeline_state_t & pipeline, const void * pVShaderByteCode, SIZE_T VShaderLength,
	const void * pPShaderByteCode, SIZE_T PShaderLength,
	const void * pGShaderByteCode, SIZE_T GShaderLength,
	const D3D11_INPUT_ELEMENT_DESC * vLayout, UINT layoutLength) {
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

void Renderer::CreateNewDeferredContext(UVector<Microsoft::WRL::ComPtr<ID3D11DeviceContext>> & m_pDeferredContexts)
{
	m_pDeferredContexts.push_back(nullptr);
	m_pWorldCommandList.push_back(nullptr);
	m_pDevice->CreateDeferredContext(NULL, m_pDeferredContexts[m_pDeferredContexts.size() - 1].GetAddressOf());
}
void Renderer::CreateRenderToTexture(UEngine::RenderToTexture & rtt, UINT width, UINT height) {
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
	m_pDevice->CreateTexture2D(&depthBuffer, nullptr, rtt.depthStencilBuffer.GetAddressOf());
	// Create depth stencil state
	m_pDevice->CreateDepthStencilState(&depthState, rtt.depthStencilState.GetAddressOf());
	// Create the depth stencil view
	m_pDevice->CreateDepthStencilView(rtt.depthStencilBuffer.Get(), // Depth stencil texture
		&descDSV, // Depth stencil desc
		rtt.depthStencilView.GetAddressOf());  // [out] Depth stencil view

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

	m_pDevice->CreateTexture2D(&desc, nullptr, &rtt.renderTargetTextureMap);

	/////////////////////// Map's Render Target
	// Setup the description of the render target view.
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = desc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	HRESULT h_ok = m_pDevice->CreateRenderTargetView(rtt.renderTargetTextureMap.Get(), &renderTargetViewDesc, &rtt.renderTargetViewMap);

	rtt.width = width;
	rtt.height = height;
	
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	// Setup the description of the shader resource view.
	D3D11_SHADER_RESOURCE_VIEW_DESC singleSRVDesc;
	singleSRVDesc.Format = desc.Format;
	singleSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	singleSRVDesc.Texture2D.MostDetailedMip = 0;
	singleSRVDesc.Texture2D.MipLevels = 1;

	m_pDevice->CreateTexture2D(&desc, nullptr, &rtt.outTexture);
	m_pDevice->CreateShaderResourceView(rtt.outTexture.Get(), &singleSRVDesc, &rtt.outSRV);
}
void Renderer::AddNewLayer(RECT clientSize) {
	D3D11_VIEWPORT rttViewPort;
#pragma region WORLD_STATIC_RTT
	CreateNewDeferredContext(m_pWorldDeferredContext);
	m_pRTT.push_back(UEngine::RenderToTexture());
	CreateRenderToTexture(m_pRTT[UEngine::DrawType_WORLD], (UINT)(clientSize.right - clientSize.left), (UINT)(clientSize.bottom - clientSize.top));

	InitViewport(rttViewPort, clientSize);
	m_pViewports.push_back(rttViewPort);
#pragma endregion
#pragma region UI_RTT
	CreateNewDeferredContext(m_pWorldDeferredContext);
	m_pRTT.push_back(UEngine::RenderToTexture());
	CreateRenderToTexture(m_pRTT[UEngine::DrawType_UI], (UINT)(clientSize.right - clientSize.left), (UINT)(clientSize.bottom - clientSize.top));
	
	InitViewport(rttViewPort, clientSize);
	m_pViewports.push_back(rttViewPort);
#pragma endregion
}
void Renderer::AddBasicPipelines() {
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
	pipeline.samplerState = default_pipeline.samplerState;
	pipeline.blendingState = default_pipeline.blendingState;
	pipeline.drawType = UEngine::DrawType_WORLD;
	m_pPipelines.push_back(pipeline);

	// PipelineType_ANIMATION
	//m_pDevice->CreateVertexShader(pVShaderByteCode, VShaderLength, nullptr, pipeline.vertex_shader.GetAddressOf());
	pipeline.vertex_shader = nullptr;
	m_pPipelines.push_back(pipeline);

	// PipelineType_UI
	pipeline.rasterState = default_pipeline.rasterState;
	pipeline.samplerState = default_pipeline.samplerState;
	pipeline.blendingState = default_pipeline.blendingState;
	pipeline.drawType = UEngine::DrawType_UI;
	pipeline.vertex_shader = default_pipeline.vertex_shader;
	pipeline.pixel_shader = default_pipeline.pixel_shader;
	pipeline.geometry_shader = default_pipeline.geometry_shader;
	pipeline.input_layout = default_pipeline.input_layout;
	m_pPipelines.push_back(pipeline);
}
void Renderer::RequestNewRTT(UEngine::RenderToTexture & rtt, UINT width, UINT height, ID3D11DeviceContext ** m_pWorldDeferredContext)
{
	CreateRenderToTexture(rtt, width, height);
	m_pDevice->CreateDeferredContext(NULL, m_pWorldDeferredContext);
}