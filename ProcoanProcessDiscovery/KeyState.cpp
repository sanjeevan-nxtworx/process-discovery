#include "stdafx.h"
#include "KeyState.h"


KeyState::KeyState()
{
	bShiftKey = false;
	bControlKey = false;
	bAltKey = false;
	bFuncKey = false;
	bWindowsKey = false;
	bCapsLock = false;
	bNumLock = false;
	bScrollLock = false;
	bMouseLButton = false;
	bMouseRButton = false;
	bMouseMButton = false;
}


KeyState::~KeyState()
{
}

string KeyState::GetControlKeyValue()
{
	string keyString = "";
	if (bMouseRButton)
	{
		keyString = "Mouse Right Button + ";
	}
	if (bMouseLButton)
	{
		keyString += "Mouse Left Button + ";
	}
	if (bMouseMButton)
	{
		keyString += "Mouse Middle Button + ";
	}
	if (bControlKey)
	{
		keyString += "Control + ";
	}
	if (bAltKey)
	{
		keyString += "Alt + ";
	}

	return keyString;
}

string KeyState::GetActionKey(int keyVal)
{
	string keyString = "";
	if (bMouseRButton || bMouseLButton || bMouseRButton
		|| bControlKey || bAltKey || bFuncKey)
		keyString = GetControlKeyValue();
	switch (keyVal)
	{
	case  VK_LWIN:
	case  VK_RWIN:
		keyString += "Start Menu";
		break;
	case  VK_PAUSE:
		keyString += "Pause";
		break;
	case  VK_ESCAPE:
		keyString += "Escape";
		break;
	case  VK_PRIOR:
		keyString += "Page Up";
		break;
	case  VK_NEXT:
		keyString += "Page Down";
		break;
	case  VK_END:
		if (bShiftKey)
			keyString = "Select Text";
		else
			keyString += "End";
		break;
	case  VK_HOME:
		if (bShiftKey)
			keyString = "Select Text";
		else
			keyString += "Home";;
		break;
	case  VK_LEFT:
		if (bShiftKey)
			keyString = "Select Text";
		else
			keyString += "Left";
		break;
	case  VK_UP:
		if (bShiftKey)
			keyString = "Select Text";
		else
			keyString += "Up";
		break;
	case  VK_RIGHT:
		if (bShiftKey)
			keyString = "Select Text";
		else
			keyString += "Right";
		break;
	case  VK_DOWN:
		if (bShiftKey)
			keyString = "Select Text";
		else
			keyString += "Down";
		break;
	case  VK_SELECT:
		keyString += "Select";
		break;
	case  VK_PRINT:
		keyString += "Print";
		break;
	case  VK_EXECUTE:
		keyString += "Execute";
		break;
	case  VK_SNAPSHOT:
		keyString += "Snapshot";
		break;
	case  VK_INSERT:
		keyString += "Insert Key";
		break;
	case  VK_DELETE:
		keyString += "Delete";
		break;
	case  VK_HELP:
		keyString += "Help";
		break;
	case  VK_APPS:
		keyString += "Apps";
		break;
	case  VK_SLEEP:
		keyString += "Sleep";
		break;
	case  VK_F1:
		keyString += "F1";
		break;
	case  VK_F2:
		keyString += "F2";
		break;
	case  VK_F3:
		keyString += "F3";
		break;
	case  VK_F4:
		keyString += "F4";
		break;
	case  VK_F5:
		keyString += "F5";
		break;
	case  VK_F6:
		keyString += "F6";
		break;
	case  VK_F7:
		keyString += "F7";
		break;
	case  VK_F8:
		keyString += "F8";
		break;
	case  VK_F9:
		keyString += "F9";
		break;
	case  VK_F10:
		keyString += "F10";
		break;
	case  VK_F11:
		keyString += "F11";
		break;
	case  VK_F12:
		keyString += "F12";
		break;
	case  VK_F13:
		keyString += "F13";
		break;
	case  VK_F14:
		keyString += "F14";
		break;
	case  VK_F15:
		keyString += "F15";
		break;
	case  VK_F16:
		keyString += "F16";
		break;
	case  VK_F17:
		keyString += "F17";
		break;
	case  VK_F18:
		keyString += "F18";
		break;
	case  VK_F19:
		keyString += "F19";
		break;
	case  VK_F20:
		keyString += "F20";
		break;
	case  VK_F21:
		keyString += "F21";
		break;
	case  VK_F22:
		keyString += "F22";
		break;
	case  VK_F23:
		keyString += "F23";
		break;
	case  VK_F24:
		keyString += "F24";
		break;
	case  VK_BACK:
		keyString += "Back";
		break;
	case  VK_TAB:
		keyString += "Tab";
		break;
	case  VK_CLEAR:
		keyString += "Clear";
		break;
	case  VK_RETURN:
		keyString += "Enter";
		break;
	}

	return keyString;
}

string KeyState::GetPrintableKey(int keyVal)
{
	string keyString = "";

	if (bMouseRButton || bMouseLButton || bMouseRButton
		|| bControlKey || bAltKey || bFuncKey)
	{
		keyString = GetControlKeyValue();
		UINT code = MapVirtualKeyA(keyVal, MAPVK_VK_TO_CHAR);
		keyString += code;
	}
	else
	{
		keyString = "*";
	}
	// This is a place to transalte keys to functions like Cntrl+C = Cut, etc.
	return keyString;
}

bool KeyState::IsCommandKeys(int keyVal)
{
	bool isCommand = false;

	switch (keyVal)
	{
	case  VK_SHIFT:
	case  VK_LSHIFT:
	case  VK_RSHIFT:
	case  VK_CONTROL:
	case  VK_LCONTROL:
	case  VK_RCONTROL:
	case  VK_MENU:
	case  VK_LMENU:
	case  VK_RMENU:
		isCommand = true;
		break;
	default:
		isCommand = false;
		break;
	}

	return isCommand;
}

bool KeyState::IsPrintableKey(int keyVal)
{
	bool isPrintable = false;

	switch (keyVal)
	{
	case  VK_SHIFT:
	case  VK_LSHIFT:
	case  VK_RSHIFT:
	case  VK_CONTROL:
	case  VK_LCONTROL:
	case  VK_RCONTROL:
	case  VK_MENU:
	case  VK_LMENU:
	case  VK_RMENU:
	case VK_NUMLOCK:
	case VK_SCROLL:
	case VK_CAPITAL:
	case  VK_LBUTTON:
	case  VK_RBUTTON:
	case VK_MBUTTON:
		isPrintable = false;
		break;
	default:
		isPrintable = true;
		break;
	}

	return isPrintable;
}

string KeyState::ProcessKey(int keyVal, int flagVal)
{
	string keyString = "";
	
	if ((flagVal & 0x80) == 0)
	{
		switch (keyVal)
		{
		case VK_NUMLOCK:
			bNumLock = !bNumLock;
			break;
		case VK_SCROLL:
			bScrollLock = !bScrollLock;
			break;
		case VK_CAPITAL:
			bCapsLock = !bCapsLock;
			break;
		case  VK_LBUTTON:
			bMouseLButton = !bMouseLButton;
			break;
		case  VK_RBUTTON:
			bMouseRButton = !bMouseRButton;
			break;
		case VK_MBUTTON:
			bMouseMButton = !bMouseMButton;
			break;
		case  VK_SHIFT:
		case  VK_LSHIFT:
		case  VK_RSHIFT:
			bShiftKey = true;
			break;
		case  VK_CONTROL:
		case  VK_LCONTROL:
		case  VK_RCONTROL:
			bControlKey = true;
			break;
		case  VK_MENU:
		case  VK_LMENU:
		case  VK_RMENU:
			bAltKey = true;
			break;
		case  VK_LWIN:
		case  VK_RWIN:
		case  VK_PAUSE:
		case  VK_ESCAPE:
		case  VK_PRIOR:
		case  VK_NEXT:
		case  VK_END:
		case  VK_HOME:
		case  VK_LEFT:
		case  VK_UP:
		case  VK_RIGHT:
		case  VK_DOWN:
		case  VK_SELECT:
		case  VK_PRINT:
		case  VK_EXECUTE:
		case  VK_SNAPSHOT:
		case  VK_INSERT:
		case  VK_DELETE:
		case  VK_HELP:
		case  VK_APPS:
		case  VK_SLEEP:
		case  VK_F1:
		case  VK_F2:
		case  VK_F3:
		case  VK_F4:
		case  VK_F5:
		case  VK_F6:
		case  VK_F7:
		case  VK_F8:
		case  VK_F9:
		case  VK_F10:
		case  VK_F11:
		case  VK_F12:
		case  VK_F13:
		case  VK_F14:
		case  VK_F15:
		case  VK_F16:
		case  VK_F17:
		case  VK_F18:
		case  VK_F19:
		case  VK_F20:
		case  VK_F21:
		case  VK_F22:
		case  VK_F23:
		case  VK_F24:
		case  VK_BACK:
		case  VK_TAB:
		case  VK_CLEAR:
		case  VK_RETURN:
			keyString = GetActionKey(keyVal);
			break;
		case  VK_NUMPAD0:
		case  VK_NUMPAD1:
		case  VK_NUMPAD2:
		case  VK_NUMPAD3:
		case  VK_NUMPAD4:
		case  VK_NUMPAD5:
		case  VK_NUMPAD6:
		case  VK_NUMPAD7:
		case  VK_NUMPAD8:
		case  VK_NUMPAD9:
			if (bNumLock)
				keyString = GetPrintableKey(keyVal);
			else
				keyString = GetActionKey(keyVal);
			break;
		default:
			keyString = GetPrintableKey(keyVal);
			break;
			
		}
	}
	else
	{
	switch (keyVal)
	{
	case  VK_SHIFT:
	case  VK_LSHIFT:
	case  VK_RSHIFT:
		bShiftKey = false;
		break;
	case  VK_CONTROL:
	case  VK_LCONTROL:
	case  VK_RCONTROL:
		bControlKey = false;
		break;
	case  VK_MENU:
		bAltKey = false;
		break;
	}
 }
	return keyString;
}


