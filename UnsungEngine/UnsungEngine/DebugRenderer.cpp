#include "stdafx.h"
#include "DebugRenderer.h"


DebugRenderer::DebugRenderer(ID3D11Device *_device, ID3D11DeviceContext *_immediateContext)
{
	// create all buffers
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.ByteWidth = (UINT)(sizeof(SIMPLE_VERTEX) * MAX_VERTS);
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	device = _device;
	m_pImmediateContext = _immediateContext;
	device->CreateBuffer(&bufferDesc, nullptr, &gpu_side_buffer);
	ZeroMemory(&cpu_side_buffer, sizeof(cpu_side_buffer));
}


DebugRenderer::~DebugRenderer()
{
	if (gpu_side_buffer)
		gpu_side_buffer->Release();
}

void DebugRenderer::Add_line(SIMPLE_VERTEX a, SIMPLE_VERTEX b)
{
	cpu_side_buffer[vert_count++] = a;
	cpu_side_buffer[vert_count++] = b;
}

void DebugRenderer::Flush()
{
	// set texture info
	ID3D11ShaderResourceView *diffuseView[]{ (ID3D11ShaderResourceView*) nullptr };
	m_pImmediateContext->PSSetShaderResources(0, 1, diffuseView);
	ID3D11ShaderResourceView *specularView[]{ (ID3D11ShaderResourceView*) nullptr };
	m_pImmediateContext->PSSetShaderResources(1, 1, specularView);
	ID3D11ShaderResourceView *emissiveView[]{ (ID3D11ShaderResourceView*) nullptr };
	m_pImmediateContext->PSSetShaderResources(2, 1, emissiveView);
	ID3D11ShaderResourceView *normalView[]{ (ID3D11ShaderResourceView*) nullptr };
	m_pImmediateContext->PSSetShaderResources(3, 1, normalView);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	m_pImmediateContext->Map(gpu_side_buffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
	memcpy(mappedResource.pData, &cpu_side_buffer, sizeof(SIMPLE_VERTEX) * vert_count);
	m_pImmediateContext->Unmap(gpu_side_buffer, 0);

	UINT stride = sizeof(SIMPLE_VERTEX);
	UINT offset = 0;

	m_pImmediateContext->IASetVertexBuffers(0, 1, &gpu_side_buffer, &stride, &offset);
	m_pImmediateContext->Draw(vert_count, 0);

	vert_count = 0;
}

