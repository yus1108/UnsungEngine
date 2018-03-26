#pragma once
class Render_World : public RenderComponent
{
private:
	Geometry * geometryComponent;
	Material * materialComponent;
	Animation * animationComponent;

	DirectX::XMMATRIX worldMat;
public:
	Render_World();
	virtual ~Render_World();

	void Init(UEngine::pipeline_state_t * pipeline, GameObject * _parent);
	void Update();
	void DrawObj(Renderer * render, Component * m_pCamera);
	void ReadBin(const char * filename, ID3D11Device * m_pDevice,
		ID3D11DeviceContext * m_pDeviceContext, DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1, 1, 1, 1));
	void CalculateCBox();
};

