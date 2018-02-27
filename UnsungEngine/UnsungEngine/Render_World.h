#pragma once
class Render_World : public RenderComponent
{
private:
	Geometry * geometryComponent;
	Material * materialComponent;
	Animation * animationComponent;
public:
	Render_World();
	virtual ~Render_World();

	virtual void Init(ID3D11DeviceContext * deviceContext, pipeline_state_t * pipeline, D3D11_VIEWPORT viewport);
	virtual void DrawObj(Renderer * render);
	virtual void ReadBin(const char * filename, ID3D11Device * m_pDevice,
		ID3D11DeviceContext * m_pDeviceContext, DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1, 1, 1, 1));
};

