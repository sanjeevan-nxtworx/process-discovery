#pragma once
class TransparentWindow
{
	HWND hTransparentWnd;

public:
	TransparentWindow();
	~TransparentWindow();
	void CreateTransparentWindow();
	HWND GetTransparentWindowHandle();
};

