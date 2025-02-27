// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <tchar.h>
#include "PecanLib.h"

/****************************************************************
*                               DllMain
* Inputs:
*       HINSTANCE hInst: Instance handle for the DLL
*       DWORD Reason: Reason for call
*       LPVOID reserved: ignored
* Result: BOOL
*       TRUE if successful
*       FALSE if there was an error (never returned)
* Effect:
*       Initializes the DLL.
****************************************************************/
HINSTANCE hInstance;
extern HWND hWndMouseServer;
extern HWND hWndKeyboardServer;
extern UINT HookMessage;

BOOL __stdcall DllMain(HINSTANCE hInst, DWORD Reason, LPVOID reserved)
{
	switch (Reason)
	{ /* reason */
	 //**********************************************
	 // PROCESS_ATTACH
	 //**********************************************
	case DLL_PROCESS_ATTACH:
		// Save the instance handle because we need it to set the hook later
		hInstance = hInst;
		HookMessage = RegisterWindowMessage(L"PECANHOOKMESSAGE");
		// This code initializes the hook notification message
		return TRUE;

		//**********************************************
		// PROCESS_DETACH
		//**********************************************
	case DLL_PROCESS_DETACH:
		// If the server has not unhooked the hook, unhook it as we unload
		if (hWndMouseServer != NULL)
			clearMouseHook(hWndMouseServer);
		if (hWndKeyboardServer != NULL)
			clearKeyboardHook(hWndKeyboardServer);
		return TRUE;
	} /* reason */
}
