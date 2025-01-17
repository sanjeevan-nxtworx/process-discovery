#pragma once
#include "..\PecanLib\PecanLib.h"

class RecorderWindow
{
	HWND hRecorderWnd;

public:
	RecorderWindow();
	~RecorderWindow();
	void CreateRecorderWindow(HWND Parent, HINSTANCE hInstance);
	HWND GetRecorderWindowHandle();
	void SetCallBack(IMouseEventCallback *callback);
};

