#pragma once
#include "Renderer.h"
class Renderer;
class RenderComponent
{
private:
	Geometry * geometryComponent;
	Material * materialComponent;
	Animation * animationComponent;
	DirectX::XMMATRIX worldMat;
	bool loadingDone;
public:
	RenderComponent();
	~RenderComponent();

	void DrawObj(ID3D11DeviceContext * m_pDeviceContext, D3D11_VIEWPORT m_viewport, Renderer * render);

	void ReadBin(const char * filename, ID3D11Device * m_pDevice,
		ID3D11DeviceContext * m_pDeviceContext, DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1, 1, 1, 1));


};

