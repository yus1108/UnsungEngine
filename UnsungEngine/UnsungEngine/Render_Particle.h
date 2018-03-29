#pragma once
class Render_Particle : public RenderComponent
{
private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pOffscreenTexture;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	m_pOffscreenRenderTarget;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pOffscreenSRV;

	UVector<UEngine::ParticleVertex> cpu_side_buffer;
	ID3D11Buffer * gpu_side_buffer = nullptr;
	UVector<UEngine::ParticleConstBuffer> particles;
	UVector<UEngine::ParticleConstBuffer> worldPos;
	UVector<DirectX::XMVECTOR> speed;
	UVector<float> duration;
	float creationCounter;
	float creationTime;
	float lifespan;
	float numEmit;
	bool isOneDirection;
	DirectX::XMVECTOR setSpeed;
	DirectX::XMFLOAT3 setPosition;
	DirectX::XMFLOAT3 setScale;
public:
	Render_Particle();
	virtual ~Render_Particle();

	unsigned GetNumParticles() { return particles.size(); }
	float GetNumEmit() { return numEmit; }
	void SetNumEmit(float _numEmit) { numEmit = _numEmit; }
	DirectX::XMVECTOR GetSpeed() { return setSpeed; }
	void SetSpeed(float x, float y, float z);
	DirectX::XMFLOAT3 GetPosition() { return setPosition; }
	void SetPosition(float x, float y, float z);
	DirectX::XMFLOAT3 GetScale() { return setScale; }
	void SetScale(float x, float y, float z);
	bool GetIsOneDirection() { return isOneDirection; }
	void SetIsOneDirection(bool _isOneDirection) { isOneDirection = _isOneDirection; }

	void Init(UEngine::pipeline_state_t * pipeline, GameObject * _parent);
	void Init(ID3D11Device * device);
	void Update();
	void DrawObj(Renderer * render, Component * m_pCamera);
	void ReadBin(const char * filename, ID3D11Device * m_pDevice,
		ID3D11DeviceContext * m_pDeviceContext, DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1, 1, 1, 1));
	void CalculateCBox();
};

