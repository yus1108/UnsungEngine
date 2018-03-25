#pragma once
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
	DebugRenderer(ID3D11Device *_device, ID3D11DeviceContext *_immediateContext);
	~DebugRenderer();

	void Add_line(UEngine::DebugVertex a, UEngine::DebugVertex b);
	void Add_AABB(AABB aabb, DirectX::XMFLOAT4 color);
	//void Add_OBB(OBB obb, DirectX::XMFLOAT4 color);
	//void Add_Frustum(Trapezoid obb, UCamera & myCam, DirectX::XMFLOAT4 color);
	//void Add_Axis(UnitNode * node);
	void Flush();
};