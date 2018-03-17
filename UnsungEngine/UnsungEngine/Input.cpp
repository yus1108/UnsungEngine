#include "stdafx.h"
#include "Input.h"

Input::Input()
{
}


Input::~Input()
{
}

bool Input::GetKey(int _key)
{
	std::unique_lock<std::mutex> inputLock(inputMutex);
	return keys[_key];
}

bool Input::GetKeyPress(int _key)
{
	std::unique_lock<std::mutex> inputLock(inputMutex);
	if (keys[_key])
	{
		counter[_key]++;
		if (counter[_key] == 1)
			return true;
		else if (counter[_key] == UINT_MAX)
			counter[_key] = 2;
	}
	else {
		counter[_key] = 0;
	}
	return false;
}

bool Input::GetMouseInput(int _mouseInput)
{
	std::unique_lock<std::mutex> inputLock(inputMutex);
	return mouse[_mouseInput];
}

bool Input::GetMousePress(int _key)
{
	std::unique_lock<std::mutex> inputLock(inputMutex);
	if (mouse[_key])
	{
		mouseCounter[_key]++;
		if (mouseCounter[_key] == 1)
			return true;
		else if (mouseCounter[_key] == UINT_MAX)
			mouseCounter[_key] = 2;
	}
	else {
		mouseCounter[_key] = 0;
	}
	return false;
}

DirectX::XMFLOAT2 Input::GetMousePos()
{
	POINT mousePos;
	GetCursorPos(&mousePos);
	RECT clientSize;
	GetClientRect(hWnd, &clientSize);
	return UMath::ConvertPixelToNDC(mousePos, &hWnd);
}

void Input::SetKeyPress(bool _keypressed, WPARAM _wParam)
{
	std::unique_lock<std::mutex> inputLock(inputMutex);
	keys[_wParam] = _keypressed;
}
void Input::SetMousePress(bool _keypressed, WPARAM _wParam)
{
	std::unique_lock<std::mutex> inputLock(inputMutex);
	mouse[_wParam] = _keypressed;
}