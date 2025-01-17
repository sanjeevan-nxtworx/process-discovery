#pragma once
#include <string>
#include <map>
#include <vector>
#include "json11.hpp"

using namespace std;
using namespace json11;

class SpyWindowLayout;
class SpyControlData;
class WindowControlData;

class SpyWindowData
{
	DWORD ID;
	string rule;
	string annotationRule;
	string windowRuleValue;
	bool bMainWindowFlg;

	string title;
	string name;
	string description;
	string className;
	string automationID;
	string type;
	long long fPos;
	long long fPosNext;
	long long fPosMask;
	long long fPosControl;

public:
	void SetID(DWORD value);
	void SetTitle(string value);
	void SetMatchingRule(string value);
	void SetAnnotationRule(string value);
	void SetName(string value);
	void SetDescription(string value);
	void SetAutomationID(string value);
	void SetClassname(string value);
	void SetWindowType(string value);
	void SetMainWindowFlg(bool value);
	bool GetMainWindowFlg();
	void SetFilePosition(long long value);
	void SetFileNextWindowPosition(long long value);
	void SetMaskFPos(long long value);
	void SetControlFPos(long long value);
	void SetWindowRuleValue(string value);

	DWORD GetID();
	string GetTitle();
	string GetMatchingRule();
	string GetAnnotationRule();
	string GetName();
	string GetDescription();
	string GetClassname();
	string GetAutomationID();
	string GetWindowType();
	string GetWindowRuleValue();

	long long GetFilePosition();
	long long GetFileNextWindowPosition();
	long long GetMaskFPos();
	long long GetControlFPos();


	SpyWindowData();
	SpyWindowData(const char *strJson);
	~SpyWindowData();
};

