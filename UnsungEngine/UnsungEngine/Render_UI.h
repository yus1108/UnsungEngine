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

	unsigned m_pCameraIndex;
	DirectX::XMFLOAT4 RTPos;
	DirectX::XMFLOAT4 RTSize;
public:
	Render_UI();
	virtual ~Render_UI();

	inline UEngine::TextFormat GetTextFormat() { return textFormat; };
	inline void SetTextFormat(UEngine::TextFormat _textFormat) { textFormat = _textFormat; };
	inline const ID3D11ShaderResourceView * const GetShaderResourceView() { return m_pOffscreenSRV.Get(); };
	inline unsigned GetCamera() { return m_pCameraIndex; }
	inline void SetCamera(unsigned camera) { m_pCameraIndex = camera; }

	void Init(ID3D11Device * m_pDevice, const WCHAR * textString, UINT32 textLength,
		const WCHAR * msc_fontName, const FLOAT msc_fontSize,
		UEngine::TextFormat textFormat = UEngine::TextFormat());
	void Init(UEngine::pipeline_state_t * pipeline, GameObject * _parent);
	void Update();
	void DrawObj(Renderer * render, Component * m_pCamera);
	void ReadBin(const char * filename, ID3D11Device * m_pDevice,
		ID3D11DeviceContext * m_pDeviceContext, DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1, 1, 1, 1));
	void ChangeText(ID3D11Device * m_pDevice, ID3D11DeviceContext * m_pImmediateContext, const WCHAR * textString, UINT32 textLength);
	void CalculateCBox();
};

