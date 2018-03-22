#pragma once
class Render_Particle : public RenderComponent
{
private:
	UVector<DirectX::XMMATRIX> particles;
public:
	Render_Particle();
	virtual ~Render_Particle();

	void Init(ID3D11Device * m_pDevice, const WCHAR * textString, UINT32 textLength,
		const WCHAR * msc_fontName, const FLOAT msc_fontSize,
		UEngine::TextFormat textFormat = UEngine::TextFormat());
	void Init(UEngine::pipeline_state_t * pipeline);
	void DrawObj(Renderer * render, Transform * transform, Component * m_pCamera);
	void ReadBin(const char * filename, ID3D11Device * m_pDevice,
		ID3D11DeviceContext * m_pDeviceContext, DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1, 1, 1, 1));
};

