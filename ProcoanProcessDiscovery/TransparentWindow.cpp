#include "stdafx.h"
#include "TransparentWindow.h"

static LRESULT CALLBACK TransparentWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

TransparentWindow::TransparentWindow()
{
	hTransparentWnd = NULL;
}


TransparentWindow::~TransparentWindow()
{
	if (hTransparentWnd != NULL)
	{
		hTransparentWnd = NULL;
		//DestroyWindow(hRecorderWnd);
	}
}

HWND TransparentWindow::GetTransparentWindowHandle()
{
	return hTransparentWnd;
}

void TransparentWindow::CreateTransparentWindow()
{
	if (hTransparentWnd != NULL)
		return;
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpszClassName = L"MyTransparentFrame";
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpfnWndProc = TransparentWindowProc;
	RegisterClassEx(&wc);
	hTransparentWnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TRANSPARENT, wc.lpszClassName, L"", WS_POPUP | WS_VISIBLE | WS_DISABLED,
		0, 0, 0, 0, nullptr, nullptr, nullptr, nullptr);
	SetLayeredWindowAttributes(hTransparentWnd, RGB(255, 128, 0), 255, LWA_COLORKEY);
	ShowWindow(hTransparentWnd, SW_HIDE);
}



static LRESULT CALLBACK
TransparentWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps{};
		HDC hdc = BeginPaint(hwnd, &ps);

		RECT rc{}; GetClientRect(hwnd, &rc);
		HPEN hPen = CreatePen(PS_SOLID, 5, RGB(255, 128, 1));
		HBRUSH hBrush = CreateSolidBrush(RGB(255, 128, 0));
		HGDIOBJ hOldPen = SelectObject(hdc, hPen);
		HGDIOBJ hOldBrush = SelectObject(hdc, hBrush);

		Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);

		if (hOldPen)
			SelectObject(hdc, hOldPen);
		if (hOldBrush)
			SelectObject(hdc, hOldBrush);
		if (hPen)
			DeleteObject(hPen);
		if (hBrush)
			DeleteObject(hBrush);

		EndPaint(hwnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProcW(hwnd, uMsg, wParam, lParam);
	}

	return 0;
}
