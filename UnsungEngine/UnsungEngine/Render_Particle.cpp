#include "stdafx.h"
#include "Render_Particle.h"


Render_Particle::Render_Particle()
{
}


Render_Particle::~Render_Particle()
{
}

void Render_Particle::Init(ID3D11Device * m_pDevice, const WCHAR * textString, UINT32 textLength, const WCHAR * msc_fontName, const FLOAT msc_fontSize, UEngine::TextFormat textFormat)
{
}

void Render_Particle::Init(UEngine::pipeline_state_t * pipeline)
{
}

void Render_Particle::DrawObj(Renderer * render, Transform * transform, Component * m_pCamera)
{
}

void Render_Particle::ReadBin(const char * filename, ID3D11Device * m_pDevice, ID3D11DeviceContext * m_pDeviceContext, DirectX::XMFLOAT4 color)
{
}
