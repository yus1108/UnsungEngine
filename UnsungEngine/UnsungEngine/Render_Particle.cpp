#include "stdafx.h"
#include "Render_Particle.h"


Render_Particle::Render_Particle() : RenderComponent()
{
	gpu_side_buffer = nullptr;
	creationCounter = 0;
	creationTime = 0.02f;
	lifespan = 1.0f;
	SetIsOneDirection(false);
	SetSpeed(0.1f, 0.1f, 0.1f);
	SetPosition(0, 0, 0);
}


Render_Particle::~Render_Particle()
{
	if (gpu_side_buffer)
		gpu_side_buffer->Release();
}

void Render_Particle::SetSpeed(float x, float y, float z)
{
	setSpeed = DirectX::XMVectorSet(x, y, z, 0);
}

void Render_Particle::SetPosition(float x, float y, float z)
{
	setPosition = DirectX::XMFLOAT3(x, y, z);
}

void Render_Particle::Init(UEngine::pipeline_state_t * pipeline, GameObject * _parent)
{
	RenderComponent::Init(pipeline, _parent);
	using namespace DirectX;
	for (unsigned i = 0; i < 10; i++)
	{
		particles.push_back(UEngine::ParticleConstBuffer());
		worldPos.push_back(UEngine::ParticleConstBuffer());
		duration.push_back(0.0f);
		speed.push_back(DirectX::XMVectorSet(
			(isOneDirection ? 1 : (rand() % 2 ? -1.0f : 1.0f)) * ((float)(rand() % 100000)) / 100000.0f,
			(isOneDirection ? 1 : (rand() % 2 ? -1.0f : 1.0f)) * ((float)(rand() % 100000)) / 100000.0f,
			(isOneDirection ? 1 : (rand() % 2 ? -1.0f : 1.0f)) * ((float)(rand() % 100000)) / 100000.0f, 0) * setSpeed);
		float sign = rand() % 2 ? -1.0f : 1.0f;
		particles[i].worldmat.x = sign * ((float)(rand() % 100000)) / 100000.0f * setPosition.x;
		sign = rand() % 2 ? -1.0f : 1.0f;
		particles[i].worldmat.y = sign * ((float)(rand() % 100000)) / 100000.0f * setPosition.y;
		sign = rand() % 2 ? -1.0f : 1.0f;
		particles[i].worldmat.z = sign * ((float)(rand() % 100000)) / 100000.0f * setPosition.z;
	}

	// Basic Model Loading
	UEngine::ParticleVertex cpu_vertex;
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
	bufferDesc.ByteWidth = (UINT)(sizeof(UEngine::ParticleVertex) * cpu_side_buffer.size());
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	device->CreateBuffer(&bufferDesc, nullptr, &gpu_side_buffer);
}

void Render_Particle::Update()
{
	if (loadingDone && isActive)
	{
		float deltaTime = (float)utime.DeltaTime();
		creationCounter += deltaTime;
		if (creationCounter >= creationTime)
		{
			using namespace DirectX;
			for (unsigned i = 0; i < 10; i++)
			{
				unsigned index = particles.size();
				if (index == 1000)
					break;

				particles.push_back(UEngine::ParticleConstBuffer());
				worldPos.push_back(UEngine::ParticleConstBuffer());
				duration.push_back(0.0f);
				speed.push_back(DirectX::XMVectorSet(
					(isOneDirection ? 1 : (rand() % 2 ? -1.0f : 1.0f)) * ((float)(rand() % 100000)) / 100000.0f,
					(isOneDirection ? 1 : (rand() % 2 ? -1.0f : 1.0f)) * ((float)(rand() % 100000)) / 100000.0f,
					(isOneDirection ? 1 : (rand() % 2 ? -1.0f : 1.0f)) * ((float)(rand() % 100000)) / 100000.0f, 0) * setSpeed);
				float sign = rand() % 2 ? -1.0f : 1.0f;
				particles[index].worldmat.x = sign * ((float)(rand() % 100000)) / 100000.0f * setPosition.x;
				sign = rand() % 2 ? -1.0f : 1.0f;
				particles[index].worldmat.y = sign * ((float)(rand() % 100000)) / 100000.0f * setPosition.y;
				sign = rand() % 2 ? -1.0f : 1.0f;
				particles[index].worldmat.z = sign * ((float)(rand() % 100000)) / 100000.0f * setPosition.z;
			}
			creationCounter = 0;
		}
		for (unsigned i = 0; i < particles.size(); i++)
		{
			using namespace DirectX;
			duration[i] += deltaTime;
			if (duration[i] >= lifespan)
			{
				particles.erase(i);
				worldPos.erase(i);
				speed.erase(i);
				duration.erase(i);
				i--;
				continue;
			}
			XMVECTOR tempPos = XMLoadFloat4(&particles[i].worldmat);
			tempPos += parent->GetTransform()->GetMatrix().r[3];
			tempPos += speed[i];
			worldPos[i] = UEngine::ParticleConstBuffer();
			XMStoreFloat4(&worldPos[i].worldmat, tempPos);
			XMStoreFloat4(&particles[i].worldmat, tempPos);
			worldPos[i].worldmat.w = 0;
			XMFLOAT3 scale = parent->GetTransform()->GetScale();
			worldPos[i].scale = DirectX::XMFLOAT4(scale.x, scale.y, 0, 0);
		}
	}
}

void Render_Particle::DrawObj(Renderer * render, Component * m_pCamera)
{
	if (loadingDone && isActive)
	{
		UINT stride = sizeof(UEngine::ParticleVertex);
		UINT offset = 0;

		CameraComponent * camera = (CameraComponent*)m_pCamera;
		ID3D11DeviceContext * deferredContext = camera->GetDeferredContext(UEngine::DrawType_WORLD);

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		deferredContext->Map(gpu_side_buffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
		memcpy(mappedResource.pData, &cpu_side_buffer[0], sizeof(UEngine::ParticleVertex) * cpu_side_buffer.size());
		deferredContext->Unmap(gpu_side_buffer, 0);

		deferredContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		deferredContext->VSSetConstantBuffers(0, 1, &render->constBufferParticleWorld);
		deferredContext->VSSetConstantBuffers(1, 1, &render->constBufferScene);
		deferredContext->GSSetConstantBuffers(0, 1, &render->constBufferParticleWorld);
		deferredContext->GSSetConstantBuffers(1, 1, &render->constBufferScene);

		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		deferredContext->Map(render->constBufferParticleWorld, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
		memcpy(mappedResource.pData, &worldPos[0], sizeof(UEngine::ParticleConstBuffer) * worldPos.size());
		deferredContext->Unmap(render->constBufferParticleWorld, 0);

		// view matrix buffer
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		deferredContext->Map(render->constBufferScene, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
		memcpy(mappedResource.pData, &camera->GetSceneToShader(), sizeof(SCENE));
		deferredContext->Unmap(render->constBufferScene, 0);

		// set texture info
		ID3D11ShaderResourceView *baseTexture[]{
			(ID3D11ShaderResourceView*)
			m_pOffscreenSRV.Get()
		};
		deferredContext->PSSetShaderResources(0, 1, baseTexture);
		deferredContext->IASetVertexBuffers(0, 1, &gpu_side_buffer, &stride, &offset);
		deferredContext->DrawInstanced(1, particles.size(), 0, 0);
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

void Render_Particle::CalculateCBox()
{
}
