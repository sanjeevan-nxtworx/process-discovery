#pragma once
#include <string>
#include <vector>
#include "SpyHeader.h"
#include "json11.hpp"

using namespace std;
using namespace json11;

class WindowControlData;

class SpyControlData
{
	DWORD controlID;

	DWORD ID;
	string name;
	string userName;
	string description;
	DWORD type;
	RECT rect;
	string className;
	string autoID;
	string controlValue;
	string screenControlType;
	bool disableFlg;
	string strClickAnnotation;
	string strEnterAnnotation;
	vector <struct matchResult *> matchResults;
	long long fPos;
	long long fPosNext;
	long long fPosParent;
	long long fPosLeftSibling;
	long long fPosRightSibling;


public:
	SpyControlData();
	~SpyControlData();

	void SetControlUserName(string value);
	string GetControlUserName();
	void SetFilePosition(long long value);

	void SetParentFilePosition(long long value);
	long long GetParentFilePosition();

	void SetLeftSiblingFilePosition(long long value);
	long long GetLeftSiblingFilePosition();

	void SetRightSiblingFilePosition(long long value);
	long long GetRightSiblingFilePosition();

	void SetFileNextControlPosition(long long value);
	long long GetFilePosition();
	long long GetFileNextControlPosition();
	string GetJSonString();
	void SetData(string str);
	void SetControlID(DWORD value);
	DWORD GetControlID();

	void SetID(DWORD value);
	DWORD GetID();

	void SetName(string value);
	string GetName();

	void SetDescription(string value);
	string GetDescription();

	void SetType(DWORD value);
	DWORD GetType();

	void SetRect(LPRECT value);
	LPRECT GetRect();

	void SetClassname(string value);
	string GetClassname();

	void SetAutoID(string value);
	string GetAutoID();

	void SetValue(string value);
	string GetValue();


	void SetScreenControlType(string value);
	string GetScreenControlType();


	void SetClickAnnotation(string value);
	string GetClickAnnotation();

	void SetEnterAnnotation(string value);
	string GetEnterAnnotation();

	string GetControlTypeName();
	string GetControlRect();

	void ClearMatchData();
	void AddMatchData(bool bPrimary, bool bRect, bool bName, bool bValue, WindowControlData *pControl);
	void DeleteMatchData(WindowControlData *pControl);
	int GetMatchCount();
	struct matchResult *GetMatchData(int num);


};

