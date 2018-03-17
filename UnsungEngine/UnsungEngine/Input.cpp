#include "stdafx.h"
#include "Input.h"

Input::Input()
{
}


Input::~Input()
{
}

void Input::SetKeyPress(bool _keypressed, WPARAM _wParam)
{
	std::unique_lock<std::mutex> newMutex(inputMutex);
	keys[_wParam] = _keypressed;
	counter[_wParam]++;
	if (_keypressed)
	{
		if (counter[_wParam] == 1)
		{
			buttonOn[_wParam] = true;
		}
		else if (counter[_wParam] == UINT_MAX)
		{
			counter[_wParam] = 1;
		}
	}
	else {
		counter[_wParam] = 0;
		buttonOn[_wParam] = false;
	}
	
}
void Input::SetMousePress(bool _keypressed, WPARAM _wParam)
{
	std::unique_lock<std::mutex> newMutex(inputMutex);
	mouse[_wParam] = _keypressed;
	if (_keypressed)
	{
		if (mouseCounter[_wParam] == 1)
		{
			mouseButtonOn[_wParam] = true;
		}
		else if (counter[_wParam] == UINT_MAX)
		{
			mouseCounter[_wParam] = 1;
		}
	}
	else {
		mouseCounter[_wParam] = 0;
		mouseButtonOn[_wParam] = false;
	}
}