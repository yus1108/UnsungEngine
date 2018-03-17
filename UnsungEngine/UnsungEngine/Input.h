#pragma once
#include <mutex>
class Input
{
private:
	std::mutex inputMutex;
	bool keys[256] = { false };
	bool buttonOn[256] = { false };
	bool mouse[3] = { false };
	bool mouseButtonOn[256] = { false };
	unsigned counter[256] = { 0 };
	unsigned mouseCounter[3] = { 0 };
public:
	POINT PrevMousePos;
	int scroll;
	bool LeftButtonClicked = false;
	bool RightButtonClicked = false;

	Input();
	~Input();

	bool GetKey(int _key) {
		std::unique_lock<std::mutex> newMutex(inputMutex);
		return keys[_key];
	}
	bool GetMouseInput(int _mouseInput) { 
		std::unique_lock<std::mutex> newMutex(inputMutex);
		return mouse[_mouseInput];
	}

	void SetKeyPress(bool _keypressed, WPARAM _wParam);
	void SetMousePress(bool _keypressed, WPARAM _wParam);
};

