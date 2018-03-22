#include "stdafx.h"
#include "Render_Particle.h"


Render_Particle::Render_Particle()
{
	gpu_side_buffer = nullptr;
}


Render_Particle::~Render_Particle()
{
	if (gpu_side_buffer)
		gpu_side_buffer->Release();
}

void Render_Particle::Init(UEngine::pipeline_state_t * pipeline)
{
	RenderComponent::Init(pipeline);
	particles.push_back(DirectX::XMVECTOR());

	// Basic Model Loading
	UEngine::ColorVertex cpu_vertex;
	cpu_vertex.pos = DirectX::XMFLOAT3(0, 5, 0);
	cpu_vertex.color = DirectX::XMFLOAT4(1, 1, 1, 1);
	cpu_side_buffer.push_back(cpu_vertex);
}

void Render_Particle::Init(ID3D11Device * device)
{
	// create all buffers
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.ByteWidth = (UINT)(sizeof(UEngine::ColorVertex) * cpu_side_buffer.size());
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	device->CreateBuffer(&bufferDesc, nullptr, &gpu_side_buffer);
}

void Render_Particle::DrawObj(Renderer * render, Transform * transform, Component * m_pCamera)
{
	if (loadingDone && isActive)
	{
		UINT stride = sizeof(UEngine::ColorVertex);
		UINT offset = 0;

		CameraComponent * camera = (CameraComponent*)m_pCamera;
		ID3D11DeviceContext * deferredContext = camera->GetDeferredContext(UEngine::DrawType_WORLD);

		float deltaTime = (float)utime.DeltaTime();
		if (deltaTime < 1000)
		{
			//cpu_side_buffer[0].pos.y -= deltaTime / 100.0f;
		}

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		deferredContext->Map(gpu_side_buffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
		memcpy(mappedResource.pData, &cpu_side_buffer[0], sizeof(UEngine::ColorVertex) * cpu_side_buffer.size());
		deferredContext->Unmap(gpu_side_buffer, 0);

		deferredContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		deferredContext->VSSetConstantBuffers(0, 1, &render->constBufferParticleWorld);
		deferredContext->VSSetConstantBuffers(1, 1, &render->constBufferScene);
		deferredContext->GSSetConstantBuffers(1, 1, &render->constBufferScene);

		UVector<DirectX::XMFLOAT4> worldPos;
		for (unsigned i = 0; i < particles.size(); i++)
		{
			using namespace DirectX;
			if (deltaTime < 1000)
			{
				particles[i].m128_f32[1] -= deltaTime / 10.0f;
				XMVECTOR tempPos = transform->GetMatrix().r[3] + particles[i];
				worldPos.push_back(XMFLOAT4());
				XMStoreFloat4(&worldPos[i], tempPos);
				worldPos[i].w = 0;
			}
			
		}

		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		deferredContext->Map(render->constBufferParticleWorld, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
		memcpy(mappedResource.pData, &worldPos[0], sizeof(DirectX::XMFLOAT4) * worldPos.size());
		deferredContext->Unmap(render->constBufferParticleWorld, 0);

		DirectX::XMMATRIX originalView = camera->GetOriginalView();
		DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant(originalView);
		float aspectRatio = ((camera->GetViewport()->Width) / (camera->GetViewport()->Height));
		camera->SetAspectRatio(aspectRatio);
		SCENE sceneToShader;
		sceneToShader.viewMat = DirectX::XMMatrixInverse(&determinant, originalView);
		sceneToShader.viewMat = DirectX::XMMatrixTranspose(sceneToShader.viewMat);
		sceneToShader.perspectivMat = DirectX::XMMatrixPerspectiveFovLH(camera->GetAngle(), aspectRatio, camera->GetNearZ(), camera->GetFarZ());
		sceneToShader.perspectivMat = DirectX::XMMatrixTranspose(sceneToShader.perspectivMat);
		camera->SetSceneToShader(sceneToShader);

		// view matrix buffer
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		deferredContext->Map(render->constBufferScene, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
		memcpy(mappedResource.pData, &sceneToShader, sizeof(SCENE));
		deferredContext->Unmap(render->constBufferScene, 0);

		// set texture info
		/*ID3D11ShaderResourceView *baseTexture[]{
			(ID3D11ShaderResourceView*)
			m_pOffscreenSRV.Get()
		};*/
		//deferredContext->PSSetShaderResources(0, 1, baseTexture);
		deferredContext->IASetVertexBuffers(0, 1, &gpu_side_buffer, &stride, &offset);
		deferredContext->Draw(1, 0);
	}
}

void Render_Particle::ReadBin(const char * filename, ID3D11Device * m_pDevice, ID3D11DeviceContext * m_pDeviceContext, DirectX::XMFLOAT4 color)
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv = nullptr;

	size_t newsize = strlen(filename) + 1;
	wchar_t * tempStr = new wchar_t[newsize];
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, tempStr, newsize, filename, _TRUNCATE);

	CreateWICTextureFromFile(m_pDevice, m_pDeviceContext, tempStr,
		(ID3D11Resource**)tex.GetAddressOf(), srv.GetAddressOf());
	if (srv) {
		m_pOffscreenSRV = srv;
		m_pOffscreenTexture = tex;
	}
	else {
		CreateDDSTextureFromFile(m_pDevice, tempStr,
			(ID3D11Resource**)tex.GetAddressOf(), srv.GetAddressOf());
		if (srv) {
			m_pOffscreenSRV = srv;
			m_pOffscreenTexture = tex;
		}
	}
	delete[] tempStr;
	loadingDone = true;
}
