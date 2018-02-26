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
#include <ctime>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <DirectXColors.h>
#include <DirectXPackedVector.h>

// reference additional headers your program requires here
#include <vector>

// utility headers
#include "WICTextureLoader.h"
#include "UVector.h"
#include "UTime.h"
#include "Definition.h"

// headers for Engine Architecture
#include "GameState.h"
#include "Renderer.h"

// include shader headers
#include "Default_VS.csh"
#include "Default_PS.csh"
#include "Default_GS.csh"