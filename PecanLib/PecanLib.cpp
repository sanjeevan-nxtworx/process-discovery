// PecanLib.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "PecanLib.h"

#pragma data_seg(".PECANLIB")
HWND hWndMouseServer = NULL;
HWND hWndKeyboardServer = NULL;
HHOOK mouseHook = NULL;
HHOOK keyboardHook = NULL;
HWND hWndServer = NULL;
UINT HookMessage = NULL;
IMouseEventCallback *pMouseEventCallback = NULL;
#pragma data_seg()
#pragma comment(linker, "/section:.PECANLIB,rws")


extern HINSTANCE hInstance;

// Forward declaration
static LRESULT CALLBACK msgMousehook(int nCode, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK msgKeyboardhook(int nCode, WPARAM wParam, LPARAM lParam);
UINT msgIDShowBitmap;

/****************************************************************
*                               setMouseCallBack
* Inputs:
*       Mouse Handler
* Result: None
*
* Effect:
*       Sets the Handler for mouse events
****************************************************************/

__declspec(dllexport) void WINAPI setMouseCallBack(IMouseEventCallback *pCallBack)
{
	pMouseEventCallback = pCallBack;
}

__declspec(dllexport) BOOL WINAPI setMouseHook(HWND hWnd)
{
	if (hWndMouseServer != NULL)
		return FALSE;
	mouseHook = SetWindowsHookEx(
		WH_MOUSE_LL,
		(HOOKPROC)msgMousehook,
		hInstance,
		0);
	if (mouseHook != NULL)
	{ /* success */
		hWndMouseServer = hWnd;
		return TRUE;
	} /* success */
	return FALSE;
} 

__declspec(dllexport) BOOL WINAPI setKeyboardHook(HWND hWnd)
{
	if (hWndKeyboardServer != NULL)
		return FALSE;
	keyboardHook = SetWindowsHookEx(
		WH_KEYBOARD_LL,
		(HOOKPROC)msgKeyboardhook,
		hInstance,
		0);
	if (keyboardHook != NULL)
	{ /* success */
		hWndKeyboardServer = hWnd;
		return TRUE;
	} /* success */
	return FALSE;
} 

__declspec(dllexport) BOOL clearMouseHook(HWND hWnd)
{
	if (hWnd != hWndMouseServer)
		return FALSE;
	BOOL unhooked = UnhookWindowsHookEx(mouseHook);
	if (unhooked)
		hWndMouseServer = NULL;
	return unhooked;
}

__declspec(dllexport) BOOL clearKeyboardHook(HWND hWnd)
{
	if (hWnd != hWndKeyboardServer)
		return FALSE;
	BOOL unhooked = UnhookWindowsHookEx(keyboardHook);
	if (unhooked)
		hWndKeyboardServer = NULL;
	return unhooked;
}


static LRESULT CALLBACK msgMousehook(int nCode, WPARAM wParam, LPARAM lParam)
{
	// If the value of nCode is < 0, just pass it on and return 0
	// this is required by the specification of hook handlers
	if (nCode < 0)
	{ /* pass it on */
		CallNextHookEx(mouseHook, nCode,
			wParam, lParam);
		return 0;
	} /* pass it on */

	// Read the documentation to discover what WPARAM and LPARAM
	// mean. For a WH_MESSAGE hook, LPARAM is specified as being
	// a pointer to a MSG structure, so the code below makes that
	// structure available

	MOUSEHOOKSTRUCT * pMouseStruct = (MOUSEHOOKSTRUCT *)lParam;
	LPARAM pt = (LPARAM)((pMouseStruct->pt.x << 16) & 0xffff0000) | (pMouseStruct->pt.y & 0xffff);

	// If it is a mouse-move message, either in the client area or
	// the non-client area, we want to notify the parent that it has
	// occurred. Note the use of PostMessage instead of SendMessage
	switch (wParam)
	{
	case WM_MOUSEMOVE:
		::PostMessage(hWndMouseServer, HookMessage, (WPARAM)30, pt);
		break;
	case WM_LBUTTONDBLCLK:
		::PostMessage(hWndMouseServer, HookMessage, (WPARAM)1, pt );
		break;
	case WM_LBUTTONDOWN:
		::PostMessage(hWndMouseServer, HookMessage, (WPARAM)2, pt);
		break;
	case WM_LBUTTONUP:
		::PostMessage(hWndMouseServer, HookMessage, (WPARAM)3, pt);
		break;
	case WM_MBUTTONDBLCLK:
		::PostMessage(hWndMouseServer, HookMessage, (WPARAM)4, pt);
		break;
	case WM_MBUTTONDOWN:
		::PostMessage(hWndMouseServer, HookMessage, (WPARAM)5, pt);
		break;
	case WM_MBUTTONUP:
		::PostMessage(hWndMouseServer, HookMessage, (WPARAM)6, pt);
		break;
	case WM_RBUTTONDBLCLK:
		::PostMessage(hWndMouseServer, HookMessage, (WPARAM)7, pt);
		break;
	case WM_RBUTTONDOWN:
		::PostMessage(hWndMouseServer, HookMessage, (WPARAM)8, pt);
		break;
	case WM_RBUTTONUP:
		::PostMessage(hWndMouseServer, HookMessage, (WPARAM)9, pt);
		break;
	}
	// Pass the message on to the next hook
	return CallNextHookEx(mouseHook, nCode,
		wParam, lParam);
} // msghook


// Hook callback 
static LRESULT CALLBACK msgKeyboardhook(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0 || nCode == HC_NOREMOVE)
		return ::CallNextHookEx(keyboardHook, nCode, wParam, lParam);

	//if (lParam & 0x40000000)    // Check the previous key state 
	//{
	//	return ::CallNextHookEx(keyboardHook, nCode, wParam, lParam);
	//}

	KBDLLHOOKSTRUCT  *pkbhs = (KBDLLHOOKSTRUCT *)lParam;

	//check that the message is from keyboard or is synthesized by SendInput API 
	if ((pkbhs->flags & LLKHF_INJECTED))
		return ::CallNextHookEx(keyboardHook, nCode, wParam, lParam);

	LPARAM keyData = ((pkbhs->vkCode << 16) & 0xffff0000) | (pkbhs->flags & 0xffff);

	if (wParam == WM_KEYDOWN || wParam == WM_KEYUP)
	{
		::PostMessage(hWndKeyboardServer, HookMessage, (WPARAM)21, keyData);
	}
	else if (wParam == WM_SYSKEYDOWN || wParam == WM_SYSKEYUP)
	{
		::PostMessage(hWndKeyboardServer, HookMessage, (WPARAM)22, keyData);
	}
	//else if (wParam == WM_KEYUP)
	//{
	//	::PostMessage(hWndKeyboardServer, HookMessage, (WPARAM)23, keyData);
	//}
	//else if (wParam == WM_SYSKEYUP)
	//{
	//	::PostMessage(hWndKeyboardServer, HookMessage, (WPARAM)24, keyData);
	//}
	return ::CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}
