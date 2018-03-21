// UnsungEngine.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "UnsungEngine.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND hWnd;
UTime utime;
GameState gameState;
Input input;
bool isTerminate;
std::mutex mainMutex;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
#ifdef _DEBUG
	//Also need this for memory leak code stuff
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetBreakAlloc(-1); //Important!
#endif // _DEBUG

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_UNSUNGENGINE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

#if TURN_CONSOLE
	AllocConsole();
	FILE* new_std_in_out;
	freopen_s(&new_std_in_out, "CONOUT$", "w", stdout);
	freopen_s(&new_std_in_out, "CONIN$", "r", stdin);
	std::cout << "Hello World!" << std::endl;
#endif // !NDEBUG

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_UNSUNGENGINE));

    MSG msg;
	
	if (lpCmdLine == NULL)
	{
		MessageBox(NULL, L"Unable to parse command line", L"Error", MB_OK);
		return 10;
	}


#pragma region arguement_handling
	//LPWSTR command = GetCommandLine();
	//size_t origsize = wcslen(command) + 1;
	//size_t convertedChars = 0;
	//char *nstring = new char[origsize];
	//// Put a copy of the converted string into nstring  
	//wcstombs_s(&convertedChars, nstring, origsize, command, _TRUNCATE);
	//UVector<std::string> arguments;
	//char * pch;
	//pch = strtok(nstring, " ");
	//while (pch != NULL)
	//{
	//	arguments.push_back(pch);
	//	size_t newsize = strlen(pch) + 1;
	//	wchar_t * wcstring = new wchar_t[newsize];

	//	// Convert char* string to a wchar_t* string.  
	//	size_t convertedChars = 0;
	//	mbstowcs_s(&convertedChars, wcstring, newsize, pch, _TRUNCATE);
	//	// Display the result and indicate the type of string that it is.  
	//	MessageBox(NULL, wcstring, L"Arglist contents", MB_OK);
	//	delete[] wcstring;
	//	pch = strtok(NULL, " ");
	//}
	//delete[] nstring;
#pragma endregion
	
	isTerminate = false;
	bool succeedTerminating = false;
	std::thread mainThreads[1];
	gameState.Init();
	UVector<void*> args;
	ThreadPool threadPool;

	
	
	mainThreads[0] = std::thread([&]() {
		while (!isTerminate)
		{
			utime.Signal();
			utime.Throttle(THROTTLE);
			
			for (int i = 0; i < NUM_THREADS; i++)
			{
				int threadNum = threadPool.AddTask((void(*)(UVector<void*>))threadPool.TestMethod, args);
				//std::cout << "thread " << threadNum << std::endl;
			}
			gameState.Update();
			for (int i = 0; i < NUM_THREADS; i++)
			{
				threadPool.Join(i);
			}
			std::unique_lock<std::mutex> mainLock(mainMutex);
		}
		std::unique_lock<std::mutex> mainLock(mainMutex);
		succeedTerminating = true;
	});
	for (auto& thread : mainThreads)
		thread.detach();

    // Main message loop:
	while (true)
	{
		// Process all messages, stop on WM_QUIT
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			// WM_QUIT does not need to be
			// translated or dispatched
			if (msg.message == WM_QUIT)
			{
				std::unique_lock<std::mutex> mainLock(mainMutex);
				isTerminate = true;
				break;
			}

			// Translates messages and sends them
			//to WndProc
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	while (!succeedTerminating)
	{
		std::unique_lock<std::mutex> mainLock(mainMutex);
	}
    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_UNSUNGENGINE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   //hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	  // CW_USEDEFAULT, 0, 800, 600, nullptr, nullptr, hInstance, nullptr);
   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
	   CW_USEDEFAULT, 0, 1280, 768, nullptr, nullptr, hInstance, nullptr);
   //hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
	  // CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	input.SetMousePress(false, UEngine::MouseInputType_SCROLL);
	switch (message)
	{
	case WM_KEYDOWN:
		input.SetKeyPress(true, wParam);
		break;
	case WM_KEYUP:
		input.SetKeyPress(false, wParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:
		input.SetMousePress(true, UEngine::MouseInputType_LEFT);
		break;
	case WM_LBUTTONUP:
		input.SetMousePress(false, UEngine::MouseInputType_LEFT);
		break;
	case WM_RBUTTONDOWN:
		input.SetMousePress(true, UEngine::MouseInputType_RIGHT);
		break;
	case WM_RBUTTONUP:
		input.SetMousePress(false, UEngine::MouseInputType_RIGHT);
		break;
	case WM_MOUSEWHEEL:
		input.SetMousePress(true, UEngine::MouseInputType_SCROLL);
		input.scroll = (int)wParam;
		break;
	case WM_MBUTTONDOWN:
		input.SetMousePress(true, UEngine::MouseInputType_MIDDLE);
		break;
	case WM_MBUTTONUP:
		input.SetMousePress(false, UEngine::MouseInputType_MIDDLE);
		break;
	case WM_SIZE:
		//gameState.SetClientSize();
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
