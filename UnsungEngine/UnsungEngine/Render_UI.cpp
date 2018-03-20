#include "stdafx.h"
#include "Render_UI.h"


Render_UI::Render_UI()
{
}


Render_UI::~Render_UI()
{
}

void Render_UI::Init(ID3D11Device * m_pDevice, const WCHAR * textString, UINT32 textLength,
	const WCHAR * msc_fontName, const FLOAT msc_fontSize,
	UEngine::TextFormat textFormat)
{
	loadingDone = false;
	HRESULT hr;

	// Create a Direct2D factory.
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, m_pDirect2dFactory.GetAddressOf());

	if (SUCCEEDED(hr))
	{

		// Create a DirectWrite factory.
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(m_pDWriteFactory.Get()),
			reinterpret_cast<IUnknown **>(m_pDWriteFactory.GetAddressOf())
		);
	}
	if (SUCCEEDED(hr))
	{
		// Create a DirectWrite text format object.
		hr = m_pDWriteFactory->CreateTextFormat(
			msc_fontName,
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			msc_fontSize,
			L"", //locale
			m_pTextFormat.GetAddressOf()
		);
	}
	if (SUCCEEDED(hr))
	{
		// Center the text horizontally and vertically.
		m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

		m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}

	if (!m_pRenderTarget)
	{
		// Allocate a offscreen D3D surface for D2D to render our 2D content into
		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.ArraySize = 1;
		texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		texDesc.Height = textFormat.width;
		texDesc.Width = textFormat.height;
		texDesc.MipLevels = 1;
		texDesc.MiscFlags = 0;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;

		/////////////////////// Map's Render Target
		// Setup the description of the render target view.
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		renderTargetViewDesc.Format = texDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		// Setup the description of the shader resource view.
		D3D11_SHADER_RESOURCE_VIEW_DESC singleSRVDesc;
		singleSRVDesc.Format = texDesc.Format;
		singleSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		singleSRVDesc.Texture2D.MostDetailedMip = 0;
		singleSRVDesc.Texture2D.MipLevels = 1;

		m_pDevice->CreateTexture2D(&texDesc, NULL, m_pOffscreenTexture.GetAddressOf());
		m_pDevice->CreateRenderTargetView(m_pOffscreenTexture.Get(), &renderTargetViewDesc, m_pOffscreenRenderTarget.GetAddressOf());
		m_pDevice->CreateShaderResourceView(m_pOffscreenTexture.Get(), &singleSRVDesc, m_pOffscreenSRV.GetAddressOf());

		// Create a Direct2D render target.
		Microsoft::WRL::ComPtr<IDXGISurface> pDxgiSurface = NULL;
		hr = m_pOffscreenTexture->QueryInterface(pDxgiSurface.GetAddressOf());
		if (SUCCEEDED(hr))
		{
			// Create a D2D render target which can draw into our offscreen D3D
			// surface. Given that we use a constant size for the texture, we
			// fix the DPI at 96.
			D2D1_RENDER_TARGET_PROPERTIES props;
			props = D2D1::RenderTargetProperties(
				D2D1_RENDER_TARGET_TYPE_DEFAULT,
				D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
				textFormat.dpiX,
				textFormat.dpiY
			);
			hr = m_pDirect2dFactory->CreateDxgiSurfaceRenderTarget(
				pDxgiSurface.Get(),
				&props,
				m_pRenderTarget.GetAddressOf()
			);
		}

		if (SUCCEEDED(hr))
		{
			// Create a blue brush.
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(textFormat.textColor),
				m_pBlackBrush.GetAddressOf()
			);
		}

		// drawing part
		if (SUCCEEDED(hr))
		{
			// Retrieve the size of the render target.
			D2D1_SIZE_F renderTargetSize = m_pRenderTarget->GetSize();

			m_pRenderTarget->BeginDraw();

			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

			if (textFormat.hasBackgroundColor)
			{
				m_pRenderTarget->Clear(D2D1::ColorF(textFormat.backgroundColor));
			}

			m_pRenderTarget->DrawText(
				textString,
				textLength,
				m_pTextFormat.Get(),
				D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height),
				m_pBlackBrush.Get()
			);

			hr = m_pRenderTarget->EndDraw();
		}
	}
	this->textFormat = textFormat;
	loadingDone = true;
}
void Render_UI::Init(UEngine::pipeline_state_t * pipeline)
{
	RenderComponent::Init(pipeline);
}

void Render_UI::DrawObj(Renderer * render, Transform * transform, Component * m_pCamera)
{
	if (loadingDone && isActive)
	{
		UINT stride = sizeof(DefaultVertex);
		UINT offset = 0;

		CameraComponent * camera = (CameraComponent*)m_pCamera;
		ID3D11DeviceContext * deferredContext = camera->GetDeferredContext(UEngine::DrawType_UI);

		deferredContext->VSSetConstantBuffers(0, 1, &render->constBufferRTTPos);
		deferredContext->GSSetConstantBuffers(1, 1, &render->constBufferRTTSize);

		DirectX::XMFLOAT4 RTPos = transform->GetPosition4();
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		deferredContext->Map(render->constBufferRTTPos, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
		memcpy(mappedResource.pData, &RTPos, sizeof(DirectX::XMFLOAT4));
		deferredContext->Unmap(render->constBufferRTTPos, 0);

		DirectX::XMFLOAT3 scale = transform->GetScale();
		DirectX::XMFLOAT4 RTSize = DirectX::XMFLOAT4(-scale.x, -scale.y, scale.x, scale.y);
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		deferredContext->Map(render->constBufferRTTSize, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
		memcpy(mappedResource.pData, &RTSize, sizeof(DirectX::XMFLOAT4));
		deferredContext->Unmap(render->constBufferRTTSize, 0);

		// set texture info
		ID3D11ShaderResourceView *baseTexture[]{
			(ID3D11ShaderResourceView*)
			m_pOffscreenSRV.Get()
		};
		deferredContext->PSSetShaderResources(0, 1, baseTexture);
		deferredContext->IASetVertexBuffers(0, 1, render->default_vertexBuffer.GetAddressOf(), &stride, &offset);
		deferredContext->Draw(1, 0);
		// to find exception: remove it when it's fixed
		camera = (CameraComponent*)m_pCamera;
	}
}

void Render_UI::ReadBin(const char * filename, ID3D11Device * m_pDevice, ID3D11DeviceContext * m_pDeviceContext, DirectX::XMFLOAT4 color)
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv = nullptr;

	size_t newsize = strlen(filename) + 1;
	wchar_t * tempStr = new wchar_t[newsize];
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, tempStr, newsize, filename, _TRUNCATE);

	CreateWICTextureFromFile(m_pDevice, m_pDeviceContext, tempStr,
		(ID3D11Resource**)tex.GetAddressOf(), srv.GetAddressOf());
	if (srv) {
		m_pOffscreenSRV = srv;
		m_pOffscreenTexture = tex;
	}
	else {
		CreateDDSTextureFromFile(m_pDevice, tempStr,
			(ID3D11Resource**)tex.GetAddressOf(), srv.GetAddressOf());
		if (srv) {
			m_pOffscreenSRV = srv;
			m_pOffscreenTexture = tex;
		}
	}
	delete[] tempStr;
	loadingDone = true;
}

void Render_UI::ChangeText(ID3D11Device * m_pDevice, ID3D11DeviceContext * m_pImmediateContext, const WCHAR * textString, UINT32 textLength)
{
	loadingDone = false;

	if (m_pOffscreenTexture)
	{
		// Retrieve the size of the render target.
		D2D1_SIZE_F renderTargetSize = m_pRenderTarget->GetSize();

		m_pImmediateContext->OMSetRenderTargets(1, m_pOffscreenRenderTarget.GetAddressOf(), nullptr);
		m_pImmediateContext->ClearRenderTargetView(m_pOffscreenRenderTarget.Get(), DirectX::Colors::Transparent);
		m_pRenderTarget->BeginDraw();

		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		if (textFormat.hasBackgroundColor)
			m_pRenderTarget->Clear(D2D1::ColorF(textFormat.backgroundColor));

		m_pRenderTarget->DrawText(
			textString,
			textLength,
			m_pTextFormat.Get(),
			D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height),
			m_pBlackBrush.Get()
		);

		HRESULT hr = m_pRenderTarget->EndDraw();
	}
	loadingDone = true;
}