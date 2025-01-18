#ifndef _DEFINED_PECANLIB
#define _DEFINED_PECANLIB
// 

#include <combaseapi.h>

#if _MSC_VER > 1000  
#pragma once
#endif

#ifdef __cplusplus  
extern "C" {
#endif 
	class IMouseEventCallback
	{
	public:
		virtual void MouseCallback(int mEventNum, ULONG lParam) = 0;
		virtual void KeyboardCallback(int mEventNum, WORD vk_code, WORD scanCode) = 0;
		virtual void TimerEnd() = 0;
	};

#ifdef _COMPILING_PECANLIB
#define LIBSPEC __declspec(dllexport)   
#else                                 
#define LIBSPEC __declspec(dllimport) 
#endif 
	LIBSPEC void WINAPI setMouseCallBack(IMouseEventCallback *pCallBack);
	LIBSPEC BOOL WINAPI setMouseHook(HWND hWnd);
	LIBSPEC BOOL clearMouseHook(HWND hWnd);
	LIBSPEC BOOL WINAPI setKeyboardHook(HWND hWnd);
	LIBSPEC BOOL clearKeyboardHook(HWND hWnd);
#undef LIBSPEC 

#ifdef __cplusplus 
}
#endif
#endif

#pragma once
