#pragma once
#include <string>
using namespace std;

class KeyState
{
	bool bShiftKey;
	bool bControlKey;
	bool bAltKey;
	bool bFuncKey;
	bool bWindowsKey;
	bool bCapsLock;
	bool bNumLock;
	bool bScrollLock;
	bool bMouseLButton;
	bool bMouseRButton;
	bool bMouseMButton;
	string GetActionKey(int keyVal);
	string GetPrintableKey(int keyVal);
	string GetControlKeyValue();

public:
	KeyState();
	~KeyState();
	string ProcessKey(int keyVal, int flagVal);
	bool IsCommandKeys(int keyVal);
	bool IsPrintableKey(int keyVal);
};

