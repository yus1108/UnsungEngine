#pragma once
#include "stdafx.h"

#define TURN_CONSOLE 1
#define THROTTLE 200

// external variables
extern HWND hWnd;
extern UTime utime;

// structure definition
struct RenderToTexture
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTargetTextureMap;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetViewMap;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViewMap;
};