#include "stdafx.h"
#include "SpyWindowData.h"
#include "SpyControlData.h"

SpyWindowData::SpyWindowData()
{
	ID = 0;
	rule = "";

	annotationRule = "";;
	bMainWindowFlg = false;

	name = "";;
	description = "";;
	className = "";;
	automationID = "";;
	type = "";
	//layouts.clear();

	fPos = NULL;
	fPosNext = NULL;
	fPosMask = NULL;
	fPosControl = NULL;
	
}

SpyWindowData::SpyWindowData(const char *strJson)
{
	fPos = NULL;
	fPosNext = NULL;
	fPosMask = NULL;
	fPosControl = NULL;

	string err;
	Json jSonElement;
	jSonElement = Json::parse(strJson, err);

	if (err == "")
	{
		SetID(jSonElement["ID"].int_value());
		SetTitle(jSonElement["Title"].string_value());
		SetName(jSonElement["WindowName"].string_value());
		SetDescription(jSonElement["Description"].string_value());
		SetClassname(jSonElement["ClassName"].string_value());
		SetAutomationID(jSonElement["AutomationID"].string_value());
		SetWindowType(jSonElement["Type"].string_value());
		SetMatchingRule(jSonElement["MatchRule"].string_value());
		SetAnnotationRule(jSonElement["AnnotationRule"].string_value());
		SetMainWindowFlg(jSonElement["IsMainWindow"].bool_value());
	}
}


SpyWindowData::~SpyWindowData()
{
	;
}



void SpyWindowData::SetFilePosition(long long value)
{
	fPos = value;
}

void SpyWindowData::SetFileNextWindowPosition(long long value)
{
	fPosNext = value;
}

void SpyWindowData::SetMaskFPos(long long value)
{
	fPosMask = value;
}

void SpyWindowData::SetControlFPos(long long value)
{
	fPosControl = value;
}


void SpyWindowData::SetID(DWORD value)
{
	ID = value;
}

void SpyWindowData::SetTitle(string value)
{
	title = value;
}

void SpyWindowData::SetWindowRuleValue(string value)
{
	windowRuleValue = value;
}

void SpyWindowData::SetMatchingRule(string value)
{
	rule = value;
}


long long SpyWindowData::GetFilePosition()
{
	return fPos;
}

long long SpyWindowData::GetFileNextWindowPosition()
{
	return fPosNext;
}

long long SpyWindowData::GetMaskFPos()
{
	return fPosMask;
}

long long SpyWindowData::GetControlFPos()
{
	return fPosControl;
}

void SpyWindowData::SetAnnotationRule(string value)
{
	annotationRule = value;
}

void SpyWindowData::SetName(string value)
{
	name = value;
}

void SpyWindowData::SetDescription(string value)
{
	description = value;
}

void SpyWindowData::SetAutomationID(string value)
{
	automationID = value;
}

void SpyWindowData::SetClassname(string value)
{
	className = value;
}
void SpyWindowData::SetMainWindowFlg(bool value)
{
	bMainWindowFlg = value;
}

bool SpyWindowData::GetMainWindowFlg()
{
	return bMainWindowFlg;
}

void SpyWindowData::SetWindowType(string value)
{
	type = value;
}

DWORD SpyWindowData::GetID()
{
	return ID;
}

string SpyWindowData::GetTitle()
{
	return title;
}

string SpyWindowData::GetMatchingRule()
{
	return rule;
}


string SpyWindowData::GetAnnotationRule()
{
	return annotationRule;
}

string SpyWindowData::GetName()
{
	return name;
}

string SpyWindowData::GetWindowRuleValue()
{
	return windowRuleValue;
}

string SpyWindowData::GetDescription()
{
	return description;
}

string SpyWindowData::GetAutomationID()
{
	return automationID;
}

string SpyWindowData::GetClassname()
{
	return className;
}


string SpyWindowData::GetWindowType()
{
	return type;
}
