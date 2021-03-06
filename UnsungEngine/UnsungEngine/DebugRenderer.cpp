#include "stdafx.h"
#include "DebugRenderer.h"


DebugRenderer::DebugRenderer()
{
	gpu_side_buffer = nullptr;
}


DebugRenderer::~DebugRenderer()
{
	if (gpu_side_buffer)
		gpu_side_buffer->Release();
}

void DebugRenderer::Init(ID3D11Device * _device, ID3D11DeviceContext * _immediateContext)
{
	if (gpu_side_buffer)
		gpu_side_buffer->Release();

	// create all buffers
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.ByteWidth = (UINT)(sizeof(UEngine::DebugVertex) * MAX_VERTS);
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	device = _device;
	m_pImmediateContext = _immediateContext;
	device->CreateBuffer(&bufferDesc, nullptr, &gpu_side_buffer);
	ZeroMemory(&cpu_side_buffer, sizeof(cpu_side_buffer));
}

void DebugRenderer::Add_line(UEngine::DebugVertex a, UEngine::DebugVertex b)
{
	cpu_side_buffer[vert_count++] = a;
	cpu_side_buffer[vert_count++] = b;
}

void DebugRenderer::Add_AABB(AABB aabb, DirectX::XMFLOAT4 color)
{
	UEngine::DebugVertex vertices[24];
	DirectX::XMFLOAT2 xaxis = aabb.GetXAxis();
	DirectX::XMFLOAT2 yaxis = aabb.GetYAxis();
	DirectX::XMFLOAT2 zaxis = aabb.GetZAxis();
	// bottom
	vertices[0].pos = DirectX::XMFLOAT3(xaxis.x, yaxis.x, zaxis.x);
	vertices[1].pos = DirectX::XMFLOAT3(xaxis.y, yaxis.x, zaxis.x);
	vertices[2].pos = DirectX::XMFLOAT3(xaxis.x, yaxis.x, zaxis.y);
	vertices[3].pos = DirectX::XMFLOAT3(xaxis.y, yaxis.x, zaxis.y);
	vertices[4].pos = DirectX::XMFLOAT3(xaxis.x, yaxis.x, zaxis.x);
	vertices[5].pos = DirectX::XMFLOAT3(xaxis.x, yaxis.x, zaxis.y);
	vertices[6].pos = DirectX::XMFLOAT3(xaxis.y, yaxis.x, zaxis.x);
	vertices[7].pos = DirectX::XMFLOAT3(xaxis.y, yaxis.x, zaxis.y);

	// middle
	vertices[8].pos = DirectX::XMFLOAT3(xaxis.x, yaxis.x, zaxis.x);
	vertices[9].pos = DirectX::XMFLOAT3(xaxis.x, yaxis.y, zaxis.x);
	vertices[10].pos = DirectX::XMFLOAT3(xaxis.y, yaxis.x, zaxis.x);
	vertices[11].pos = DirectX::XMFLOAT3(xaxis.y, yaxis.y, zaxis.x);
	vertices[12].pos = DirectX::XMFLOAT3(xaxis.y, yaxis.x, zaxis.y);
	vertices[13].pos = DirectX::XMFLOAT3(xaxis.y, yaxis.y, zaxis.y);
	vertices[14].pos = DirectX::XMFLOAT3(xaxis.x, yaxis.x, zaxis.y);
	vertices[15].pos = DirectX::XMFLOAT3(xaxis.x, yaxis.y, zaxis.y);

	// top
	vertices[16].pos = DirectX::XMFLOAT3(xaxis.x, yaxis.y, zaxis.x);
	vertices[17].pos = DirectX::XMFLOAT3(xaxis.y, yaxis.y, zaxis.x);
	vertices[18].pos = DirectX::XMFLOAT3(xaxis.x, yaxis.y, zaxis.y);
	vertices[19].pos = DirectX::XMFLOAT3(xaxis.y, yaxis.y, zaxis.y);
	vertices[20].pos = DirectX::XMFLOAT3(xaxis.x, yaxis.y, zaxis.x);
	vertices[21].pos = DirectX::XMFLOAT3(xaxis.x, yaxis.y, zaxis.y);
	vertices[22].pos = DirectX::XMFLOAT3(xaxis.y, yaxis.y, zaxis.x);
	vertices[23].pos = DirectX::XMFLOAT3(xaxis.y, yaxis.y, zaxis.y);

	for (unsigned int i = 0; i < 24; i++) {
		cpu_side_buffer[vert_count] = vertices[i];
		cpu_side_buffer[vert_count++].color = color;
	}
}
void DebugRenderer::Add_OOBB(OOBB * oobb, DirectX::XMFLOAT4 color)
{
	DirectX::XMVECTOR vertices[24];
	DirectX::XMFLOAT2 xaxis = oobb->GetXAxis();
	DirectX::XMFLOAT2 yaxis = oobb->GetYAxis();
	DirectX::XMFLOAT2 zaxis = oobb->GetZAxis();
	// bottom
	vertices[0] = DirectX::XMVectorSet(xaxis.x, yaxis.x,zaxis.x, 1);
	vertices[1] = DirectX::XMVectorSet(xaxis.y, yaxis.x,zaxis.x, 1);
	vertices[2] = DirectX::XMVectorSet(xaxis.x, yaxis.x,zaxis.y, 1);
	vertices[3] = DirectX::XMVectorSet(xaxis.y, yaxis.x,zaxis.y, 1);
	vertices[4] = DirectX::XMVectorSet(xaxis.x, yaxis.x,zaxis.x, 1);
	vertices[5] = DirectX::XMVectorSet(xaxis.x, yaxis.x,zaxis.y, 1);
	vertices[6] = DirectX::XMVectorSet(xaxis.y, yaxis.x,zaxis.x, 1);
	vertices[7] = DirectX::XMVectorSet(xaxis.y, yaxis.x,zaxis.y, 1);

	// middle
	vertices[8] = DirectX::XMVectorSet(xaxis.x, yaxis.x, zaxis.x, 1);
	vertices[9] = DirectX::XMVectorSet(xaxis.x, yaxis.y, zaxis.x, 1);
	vertices[10] = DirectX::XMVectorSet(xaxis.y, yaxis.x, zaxis.x, 1);
	vertices[11] = DirectX::XMVectorSet(xaxis.y, yaxis.y, zaxis.x, 1);
	vertices[12] = DirectX::XMVectorSet(xaxis.y, yaxis.x, zaxis.y, 1);
	vertices[13] = DirectX::XMVectorSet(xaxis.y, yaxis.y, zaxis.y, 1);
	vertices[14] = DirectX::XMVectorSet(xaxis.x, yaxis.x, zaxis.y, 1);
	vertices[15] = DirectX::XMVectorSet(xaxis.x, yaxis.y, zaxis.y, 1);

	// top
	vertices[16] = DirectX::XMVectorSet(xaxis.x, yaxis.y, zaxis.x, 1);
	vertices[17] = DirectX::XMVectorSet(xaxis.y, yaxis.y, zaxis.x, 1);
	vertices[18] = DirectX::XMVectorSet(xaxis.x, yaxis.y, zaxis.y, 1);
	vertices[19] = DirectX::XMVectorSet(xaxis.y, yaxis.y, zaxis.y, 1);
	vertices[20] = DirectX::XMVectorSet(xaxis.x, yaxis.y, zaxis.x, 1);
	vertices[21] = DirectX::XMVectorSet(xaxis.x, yaxis.y, zaxis.y, 1);
	vertices[22] = DirectX::XMVectorSet(xaxis.y, yaxis.y, zaxis.x, 1);
	vertices[23] = DirectX::XMVectorSet(xaxis.y, yaxis.y, zaxis.y, 1);

	for (unsigned int i = 0; i < 24; i++) {
		vertices[i] = DirectX::XMVector4Transform(vertices[i], oobb->GetWorldMat());
		DirectX::XMStoreFloat3(&cpu_side_buffer[vert_count].pos, vertices[i]);
		cpu_side_buffer[vert_count++].color = color;
	}
}
void DebugRenderer::Add_Frustum(Frustum frustum, DirectX::XMFLOAT4 color) {
	DirectX::XMVECTOR vertices[8];

	using namespace DirectX;
	XMMATRIX normalizedMat = frustum.GetWorldMat();
	normalizedMat.r[0] = XMVector3Normalize(normalizedMat.r[0]);
	normalizedMat.r[1] = XMVector3Normalize(normalizedMat.r[1]);
	normalizedMat.r[2] = XMVector3Normalize(normalizedMat.r[2]);
	for (unsigned int i = 0; i < 8; i++) {
		vertices[i] = DirectX::XMVector4Transform(frustum.GetVertex(i), normalizedMat);
	}

	DirectX::XMStoreFloat3(&cpu_side_buffer[vert_count].pos, vertices[0]);
	cpu_side_buffer[vert_count++].color = color;
	DirectX::XMStoreFloat3(&cpu_side_buffer[vert_count].pos, vertices[1]);
	cpu_side_buffer[vert_count++].color = color;

	DirectX::XMStoreFloat3(&cpu_side_buffer[vert_count].pos, vertices[0]);
	cpu_side_buffer[vert_count++].color = color;
	DirectX::XMStoreFloat3(&cpu_side_buffer[vert_count].pos, vertices[2]);
	cpu_side_buffer[vert_count++].color = color;

	DirectX::XMStoreFloat3(&cpu_side_buffer[vert_count].pos, vertices[1]);
	cpu_side_buffer[vert_count++].color = color;
	DirectX::XMStoreFloat3(&cpu_side_buffer[vert_count].pos, vertices[3]);
	cpu_side_buffer[vert_count++].color = color;

	DirectX::XMStoreFloat3(&cpu_side_buffer[vert_count].pos, vertices[2]);
	cpu_side_buffer[vert_count++].color = color;
	DirectX::XMStoreFloat3(&cpu_side_buffer[vert_count].pos, vertices[3]);
	cpu_side_buffer[vert_count++].color = color;

	DirectX::XMStoreFloat3(&cpu_side_buffer[vert_count].pos, vertices[0]);
	cpu_side_buffer[vert_count++].color = color;
	DirectX::XMStoreFloat3(&cpu_side_buffer[vert_count].pos, vertices[4]);
	cpu_side_buffer[vert_count++].color = color;

	DirectX::XMStoreFloat3(&cpu_side_buffer[vert_count].pos, vertices[1]);
	cpu_side_buffer[vert_count++].color = color;
	DirectX::XMStoreFloat3(&cpu_side_buffer[vert_count].pos, vertices[5]);
	cpu_side_buffer[vert_count++].color = color;

	DirectX::XMStoreFloat3(&cpu_side_buffer[vert_count].pos, vertices[4]);
	cpu_side_buffer[vert_count++].color = color;
	DirectX::XMStoreFloat3(&cpu_side_buffer[vert_count].pos, vertices[5]);
	cpu_side_buffer[vert_count++].color = color;

	DirectX::XMStoreFloat3(&cpu_side_buffer[vert_count].pos, vertices[2]);
	cpu_side_buffer[vert_count++].color = color;
	DirectX::XMStoreFloat3(&cpu_side_buffer[vert_count].pos, vertices[6]);
	cpu_side_buffer[vert_count++].color = color;

	DirectX::XMStoreFloat3(&cpu_side_buffer[vert_count].pos, vertices[3]);
	cpu_side_buffer[vert_count++].color = color;
	DirectX::XMStoreFloat3(&cpu_side_buffer[vert_count].pos, vertices[7]);
	cpu_side_buffer[vert_count++].color = color;

	DirectX::XMStoreFloat3(&cpu_side_buffer[vert_count].pos, vertices[6]);
	cpu_side_buffer[vert_count++].color = color;
	DirectX::XMStoreFloat3(&cpu_side_buffer[vert_count].pos, vertices[7]);
	cpu_side_buffer[vert_count++].color = color;

	DirectX::XMStoreFloat3(&cpu_side_buffer[vert_count].pos, vertices[4]);
	cpu_side_buffer[vert_count++].color = color;
	DirectX::XMStoreFloat3(&cpu_side_buffer[vert_count].pos, vertices[6]);
	cpu_side_buffer[vert_count++].color = color;

	DirectX::XMStoreFloat3(&cpu_side_buffer[vert_count].pos, vertices[5]);
	cpu_side_buffer[vert_count++].color = color;
	DirectX::XMStoreFloat3(&cpu_side_buffer[vert_count].pos, vertices[7]);
	cpu_side_buffer[vert_count++].color = color;
}
void DebugRenderer::Add_UI_AABB(AABB aabb, DirectX::XMFLOAT4 color)
{
	DirectX::XMVECTOR pos[24];
	UEngine::DebugVertex vertices[24];
	DirectX::XMFLOAT2 xaxis = aabb.GetXAxis();
	DirectX::XMFLOAT2 yaxis = aabb.GetYAxis();
	DirectX::XMFLOAT2 zaxis = aabb.GetZAxis();
	// bottom
	pos[0] = DirectX::XMVectorSet(xaxis.x, yaxis.x, zaxis.x, 1);
	pos[1] = DirectX::XMVectorSet(xaxis.y, yaxis.x, zaxis.x, 0);
	pos[2] = DirectX::XMVectorSet(xaxis.x, yaxis.x, zaxis.y, 0);
	pos[3] = DirectX::XMVectorSet(xaxis.y, yaxis.x, zaxis.y, 0);
	pos[4] = DirectX::XMVectorSet(xaxis.x, yaxis.x, zaxis.x, 0);
	pos[5] = DirectX::XMVectorSet(xaxis.x, yaxis.x, zaxis.y, 0);
	pos[6] = DirectX::XMVectorSet(xaxis.y, yaxis.x, zaxis.x, 0);
	pos[7] = DirectX::XMVectorSet(xaxis.y, yaxis.x, zaxis.y, 0);

	// middle
	pos[8] = DirectX::XMVectorSet(xaxis.x, yaxis.x, zaxis.x, 1);
	pos[9] = DirectX::XMVectorSet(xaxis.x, yaxis.y, zaxis.x, 1);
	pos[10] = DirectX::XMVectorSet(xaxis.y, yaxis.x, zaxis.x, 1);
	pos[11] = DirectX::XMVectorSet(xaxis.y, yaxis.y, zaxis.x, 1);
	pos[12] = DirectX::XMVectorSet(xaxis.y, yaxis.x, zaxis.y, 1);
	pos[13] = DirectX::XMVectorSet(xaxis.y, yaxis.y, zaxis.y, 1);
	pos[14] = DirectX::XMVectorSet(xaxis.x, yaxis.x, zaxis.y, 1);
	pos[15] = DirectX::XMVectorSet(xaxis.x, yaxis.y, zaxis.y, 1);

	// top
	pos[16] = DirectX::XMVectorSet(xaxis.x, yaxis.y, zaxis.x, 1);
	pos[17] = DirectX::XMVectorSet(xaxis.y, yaxis.y, zaxis.x, 1);
	pos[18] = DirectX::XMVectorSet(xaxis.x, yaxis.y, zaxis.y, 1);
	pos[19] = DirectX::XMVectorSet(xaxis.y, yaxis.y, zaxis.y, 1);
	pos[20] = DirectX::XMVectorSet(xaxis.x, yaxis.y, zaxis.x, 1);
	pos[21] = DirectX::XMVectorSet(xaxis.x, yaxis.y, zaxis.y, 1);
	pos[22] = DirectX::XMVectorSet(xaxis.y, yaxis.y, zaxis.x, 1);
	pos[23] = DirectX::XMVectorSet(xaxis.y, yaxis.y, zaxis.y, 1);

	for (unsigned int i = 0; i < 24; i++) {
		DirectX::XMMATRIX perspective = DirectX::XMMatrixTranspose(gameState.renderer->m_pCameras[0]->GetSceneToShader().perspectivMat);
		pos[i] = DirectX::XMVector3Transform(pos[i], gameState.renderer->m_pCameras[0]->GetOriginalView());
		pos[i] = DirectX::XMVector3Transform(pos[i], DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(perspective), perspective));
		vertices[i].pos = DirectX::XMFLOAT3(pos[i].m128_f32[0], pos[i].m128_f32[1], pos[i].m128_f32[2]);
		cpu_side_buffer[vert_count] = vertices[i];
		cpu_side_buffer[vert_count++].color = color;
	}
}

void DebugRenderer::Flush(ID3D11DeviceContext * m_pDeviceContext)
{
	// set texture info
	ID3D11ShaderResourceView *diffuseView[]{ (ID3D11ShaderResourceView*) nullptr };
	m_pDeviceContext->PSSetShaderResources(0, 1, diffuseView);
	ID3D11ShaderResourceView *specularView[]{ (ID3D11ShaderResourceView*) nullptr };
	m_pDeviceContext->PSSetShaderResources(1, 1, specularView);
	ID3D11ShaderResourceView *emissiveView[]{ (ID3D11ShaderResourceView*) nullptr };
	m_pDeviceContext->PSSetShaderResources(2, 1, emissiveView);
	ID3D11ShaderResourceView *normalView[]{ (ID3D11ShaderResourceView*) nullptr };
	m_pDeviceContext->PSSetShaderResources(3, 1, normalView);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	m_pDeviceContext->Map(gpu_side_buffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
	memcpy(mappedResource.pData, &cpu_side_buffer, sizeof(UEngine::DebugVertex) * vert_count);
	m_pDeviceContext->Unmap(gpu_side_buffer, 0);

	UINT stride = sizeof(UEngine::DebugVertex);
	UINT offset = 0;

	m_pDeviceContext->IASetVertexBuffers(0, 1, &gpu_side_buffer, &stride, &offset);
	m_pDeviceContext->Draw(vert_count, 0);

	vert_count = 0;
}

