#pragma once
class Render_UI : public RenderComponent
{
private:
	Microsoft::WRL::ComPtr<ID2D1Factory> m_pDirect2dFactory;
	Microsoft::WRL::ComPtr<ID2D1RenderTarget> m_pRenderTarget;
	Microsoft::WRL::ComPtr<IDWriteFactory> m_pDWriteFactory;
	Microsoft::WRL::ComPtr<IDWriteTextFormat> m_pTextFormat;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pBlackBrush;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pOffscreenTexture;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	m_pOffscreenRenderTarget;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pOffscreenSRV;
	UEngine::TextFormat textFormat;
public:
	Render_UI();
	virtual ~Render_UI();

	inline const ID3D11ShaderResourceView * const GetShaderResourceView() { return m_pOffscreenSRV.Get(); };

	void Init(ID3D11Device * m_pDevice, const WCHAR * textString, UINT32 textLength,
		const WCHAR * msc_fontName, const FLOAT msc_fontSize,
		UEngine::TextFormat textFormat = UEngine::TextFormat());
	void Init(ID3D11DeviceContext * deviceContext, pipeline_state_t * pipeline, D3D11_VIEWPORT viewport);
	void DrawObj(Renderer * render);
	void ReadBin(const char * filename, ID3D11Device * m_pDevice,
		ID3D11DeviceContext * m_pDeviceContext, DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1, 1, 1, 1));
	void ChangeText(ID3D11Device * m_pDevice, ID3D11DeviceContext * m_pImmediateContext, const WCHAR * textString, UINT32 textLength);
};

