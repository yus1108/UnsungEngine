#pragma once
#include <mutex>
class Input
{
private:
	std::mutex inputMutex;
	bool keys[256] = { false };
	bool mouse[3] = { false };
	unsigned counter[256] = { 0 };
	unsigned mouseCounter[3] = { 0 };
public:
	POINT PrevMousePos;
	int scroll;
	bool LeftButtonClicked = false;
	bool RightButtonClicked = false;

	Input();
	~Input();

	bool GetKey(int _key); // check the state key is being pressed
	bool GetKeyPress(int _key); // check the state key pressed for one time while pressed down
	bool GetMouseInput(int _mouseInput);
	bool GetMousePress(int _key); // check the state key pressed for one time while pressed down
	DirectX::XMFLOAT2 GetMousePos();

	void SetKeyPress(bool _keypressed, WPARAM _wParam);
	void SetMousePress(bool _keypressed, WPARAM _wParam);
};

