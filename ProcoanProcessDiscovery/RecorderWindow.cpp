#include "stdafx.h"
#include "RecorderWindow.h"

const WCHAR *szWindowClass = L"RecorderWindowClass";            // the main window class name
static LRESULT CALLBACK RecorderWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static UINT HookMessage;
static IMouseEventCallback *pCallback;

RecorderWindow::RecorderWindow()
{
	pCallback = NULL;
	hRecorderWnd = NULL;
}


RecorderWindow::~RecorderWindow()
{
	if (hRecorderWnd != NULL)
	{
		KillTimer(hRecorderWnd,    // handle to main window 
			1001);				 // timer identifier 

		hRecorderWnd = NULL;
		//DestroyWindow(hRecorderWnd);
	}
}

void RecorderWindow::SetCallBack(IMouseEventCallback *callback)
{
	pCallback = callback;
}

HWND RecorderWindow::GetRecorderWindowHandle()
{
	return hRecorderWnd;
}

void RecorderWindow::CreateRecorderWindow(HWND parent, HINSTANCE hInstance)
{
	if (hRecorderWnd != NULL)
		return;
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = RecorderWindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"RecorderWindowClass";
	wcex.hIconSm = NULL;

	if (!RegisterClassExW(&wcex))
		return;
	HookMessage = RegisterWindowMessage(L"PECANHOOKMESSAGE");
	hRecorderWnd = CreateWindow(szWindowClass, L"Recorder", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 300, 200, NULL, NULL, hInstance, NULL);

	if (hRecorderWnd == NULL)
	{
		return;
	}

	ShowWindow(hRecorderWnd, SW_HIDE);
	UpdateWindow(hRecorderWnd);
}



static LRESULT CALLBACK
RecorderWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int scan, key;

	if (uMsg == HookMessage)
	{
		switch (wParam)
		{
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 30:
			if (pCallback != NULL)
				pCallback->MouseCallback(wParam, (ULONG)lParam);
			break;
		case 21:
		case 22:
		case 23:
		case 24:
			scan = lParam & 0xffff;
			key = (lParam >> 16) & 0xffff;
			if (pCallback != NULL)
				pCallback->KeyboardCallback(wParam, key, scan);
			break;
		}
		return 0;
	}

	switch (uMsg) {
		case WM_CLOSE:
			DestroyWindow(hwnd);
			return 0;
			break;
			
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
			break;

		case WM_PAINT:
			{
				return DefWindowProc(hwnd, uMsg, wParam, lParam);
			}
			break;
		}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


