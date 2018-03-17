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
	std::unique_lock<std::mutex> newMutex(inputMutex);
	return keys[_key];
}

bool Input::GetKeyPress(int _key)
{
	std::unique_lock<std::mutex> newMutex(inputMutex);
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
	std::unique_lock<std::mutex> newMutex(inputMutex);
	return mouse[_mouseInput];
}

bool Input::GetMousePress(int _key)
{
	std::unique_lock<std::mutex> newMutex(inputMutex);
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

void Input::SetKeyPress(bool _keypressed, WPARAM _wParam)
{
	std::unique_lock<std::mutex> newMutex(inputMutex);
	keys[_wParam] = _keypressed;
}
void Input::SetMousePress(bool _keypressed, WPARAM _wParam)
{
	std::unique_lock<std::mutex> newMutex(inputMutex);
	mouse[_wParam] = _keypressed;
}