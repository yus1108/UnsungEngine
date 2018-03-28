#pragma once
#include "stdafx.h"
#include "UTime.h"
#include "Input.h"
#include "GameState.h"
#include "SystemDefinition.h"

#ifdef _DEBUG
#include "DebugRenderer.h"
extern DebugRenderer debugRenderer;
#endif

#define TURN_CONSOLE 1
#define THROTTLE 200
// max 5 threads per camera
#define NUM_THREADS 100
#define XBOUNDARY DirectX::XMFLOAT2(-10, 10)
#define YBOUNDARY DirectX::XMFLOAT2(-10, 10)
#define ZBOUNDARY DirectX::XMFLOAT2(-10, 10)
#define COLLISION_LEVEL 2

// external variables
extern HWND hWnd;
extern UTime utime;
extern Input input;
extern GameState gameState;

// structure definition


