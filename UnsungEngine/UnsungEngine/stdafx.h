// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#endif // _DEBUG

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <wrl/client.h>
#include <thread>
#include <mutex>
#include <iostream>

// D3D11
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <DirectXColors.h>
#include <DirectXPackedVector.h>

// D2D1
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <wchar.h>
#include <math.h>
#include <functional>

// reference additional headers your program requires here
#include <array>
#include <ctime>
#include <vector>
#include <fstream>
#include <sstream>
#include <bitset>
#include <unordered_map>
#include <random>

// utility headers
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"
#include "UVector.h"
#include "UTime.h"
#include "UMath.h"
#include "ThreadInfo.h"
#include "ThreadPool.h"
#include "Input.h"
#include "SystemDefinition.h"
#include "Definition.h"

// collision
#include "CollisionBox.h"
#include "AABB.h"
#include "OOBB.h"
#include "Frustum.h"

// headers for Engine Architecture
#include "GameState.h"
#include "GameObject.h"
#include "Renderer.h"
#ifdef _DEBUG
#include "DebugRenderer.h"
#endif

// components
#include "Component.h"
#include "CameraComponent.h"
#include "ScriptComponent.h"
#include "CollisionComponent.h"

// render components
#include "RenderComponent.h"
#include "Render_World.h"
#include "Render_UI.h"
#include "Render_Particle.h"
#include "Transform.h"

// Managers
#include "ObjectManager.h"
#include "CollisionManager.h"

// include shader headers
#include "Shaders/Default_VS.csh"
#include "Shaders/Default_PS.csh"
#include "Shaders/Default_GS.csh"
#include "Shaders/Static_VS.csh"
#include "Shaders/Static_PS.csh"
#include "Shaders/Particle_VS.csh"
#include "Shaders/Particle_GS.csh"
#include "Shaders/Particle_PS.csh"
#ifdef _DEBUG
#include "Shaders/DebugRender_VS.csh"
#include "Shaders/DebugRender_PS.csh"
#endif

