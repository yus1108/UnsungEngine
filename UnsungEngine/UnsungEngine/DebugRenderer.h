#pragma once
#include "OOBB.h"
class DebugRenderer
{
private:
	// taking it from graphics system
	ID3D11Device *device;
	ID3D11DeviceContext *m_pImmediateContext;

	static const int MAX_VERTS = 100000;
	ID3D11Buffer *gpu_side_buffer = nullptr;
	UEngine::DebugVertex cpu_side_buffer[MAX_VERTS];
	int vert_count = 0;
public:
	DebugRenderer();
	~DebugRenderer();

	void Init(ID3D11Device *_device, ID3D11DeviceContext *_immediateContext);
	void Add_line(UEngine::DebugVertex a, UEngine::DebugVertex b);
	void Add_AABB(AABB aabb, DirectX::XMFLOAT4 color);
	void Add_OOBB(OOBB * oobb, DirectX::XMFLOAT4 color);
	void Add_Frustum(Frustum frustum, DirectX::XMFLOAT4 color);
	//void Add_Axis(UnitNode * node);
	void Flush(ID3D11DeviceContext * m_pDeviceContext);
};