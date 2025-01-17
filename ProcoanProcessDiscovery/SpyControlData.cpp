#include "stdafx.h"
#include "SpyControlData.h"
#include <UIAutomationClient.h>
#include "Utility.h"

SpyControlData::SpyControlData()
{
	controlID = 0L;
	ID = 0L;
	name = "";
	description = "";;
	type = 0L;
	ZeroMemory(&rect, sizeof(RECT));
	className = "";;
	autoID = "";;
	controlValue = "";
	disableFlg = true;
	strClickAnnotation = "";
	strEnterAnnotation = "";
	fPos = NULL;
	fPosNext = NULL;

	matchResults.clear();
}


SpyControlData::~SpyControlData()
{
	ClearMatchData();
}

void SpyControlData::SetFilePosition(long long value)
{
	fPos = value;
}

void SpyControlData::SetFileNextControlPosition(long long value)
{
	fPosNext = value;
}

long long SpyControlData::GetFilePosition()
{
	return fPos;
}

long long SpyControlData::GetFileNextControlPosition()
{
	return fPosNext;
}

void SpyControlData::SetData(string strJSON)
{

	/*
	{
			"ControlID": xxx,
			"ID": XXX,
			"Name": "Control Name",
			"Description": "Control Description",
			"Type": xxxx,
			"Rect": {
				"Left": xxx,
				"Top": xxx,
				"Right": xxx,
				"Bottom": xxx
				},
			"ClassName": "Control Classname",
			"AutoID": "Control Automation ID",
			"ClickAnnotation": "Control Automation ID",
			"EnterAnnotation": "Control Automation ID",
			"UseRect": true,
			"UseName": true,
			"UseValue": true,
			"Disable": true,
			"CreatesWindow": true,
			"ClosesWindow": true
			}
	*/

	string err;
	Json jSonElement;

	jSonElement = Json::parse(strJSON, err);
	if (err != "")
		return;

	SetControlID(jSonElement["ControlID"].int_value());
	SetID(jSonElement["ID"].int_value());
	SetName(jSonElement["Name"].string_value());
	SetDescription(jSonElement["Description"].string_value());
	map <string, Json> rectData = jSonElement["Rect"].object_items();
	map <string, Json>::iterator itRectData = rectData.begin();
	
	RECT rectElement;
	ZeroMemory(&rectElement, sizeof(RECT));

	while (itRectData != rectData.end())
	{
		Json rectVal = itRectData->second;
		string key = itRectData->first;
		if (key == "Left")
			rectElement.left = rectVal.int_value();
		else if (key == "Top")
			rectElement.top = rectVal.int_value();
		else if (key == "Right")
			rectElement.right = rectVal.int_value();
		else if (key == "Bottom")
			rectElement.bottom = rectVal.int_value();
		itRectData++;
	}

	SetRect(&rectElement);
	SetType(jSonElement["Type"].int_value());
	SetClassname (jSonElement["ClassName"].string_value());
	SetAutoID(jSonElement["AutoID"].string_value());
	SetControlUserName(jSonElement["UserName"].string_value());
	SetClickAnnotation(jSonElement["ClickAnnotation"].string_value());
	SetEnterAnnotation(jSonElement["EnterAnnotation"].string_value());
	SetScreenControlType(jSonElement["ControlType"].string_value());

}

string SpyControlData::GetJSonString()
{
	string JSONstr = "{";
	char str[100];

	JSONstr = JSONstr + "\"ControlID\": ";
	sprintf_s(str, "%d,", GetControlID());
	JSONstr = JSONstr + str;
	sprintf_s(str, "\"ID\":%d,", GetID());
	JSONstr = JSONstr + str;

	JSONstr = JSONstr + "\"Name\": \"" + EscapeJSonAnnotationString(GetName()) + "\",";
	JSONstr = JSONstr + "\"Description\": \"" + EscapeJSonAnnotationString(GetDescription()) + "\",";
	sprintf_s(str, "\"Type\":%d,", GetType());
	JSONstr = JSONstr + str;
	JSONstr = JSONstr + "\"ControlType\": \"" + GetScreenControlType() + "\",";
	sprintf_s(str, "\"Rect\": { \"Left\": %d, \"Top\": %d, \"Right\": %d, \"Bottom\": %d},",
			GetRect()->left,
			GetRect()->top,
			GetRect()->right,
			GetRect()->bottom);
	JSONstr = JSONstr + str;
	JSONstr = JSONstr + "\"ClassName\": \"" + EscapeJSonAnnotationString(GetClassname()) + "\",";
	JSONstr = JSONstr + "\"AutoID\": \"" + EscapeJSonAnnotationString(GetAutoID()) + "\",";
	JSONstr = JSONstr + "\"UserName\": \"" + EscapeJSonAnnotationString(GetControlUserName()) + "\",";
	JSONstr = JSONstr + "\"ClickAnnotation\": \"" + EscapeJSonAnnotationString(GetClickAnnotation()) + "\",";
	JSONstr = JSONstr + "\"EnterAnnotation\": \"" + EscapeJSonAnnotationString(GetEnterAnnotation()) + "\"";
	JSONstr += "}";

	return JSONstr;
}


void SpyControlData::SetControlID(DWORD value)
{
	controlID = value;
}

DWORD SpyControlData::GetControlID()
{
	return controlID;
}

void SpyControlData::SetID(DWORD value)
{
	ID = value;
}

DWORD SpyControlData::GetID()
{
	return ID;
}

void SpyControlData::SetName(string value)
{
	name = value;
}

string SpyControlData::GetName()
{
	return name;
}

void SpyControlData::SetControlUserName(string value)
{
	userName = value;
}

string SpyControlData::GetControlUserName()
{
	return userName;
}


void SpyControlData::SetDescription(string value)
{
	description = value;
}

string SpyControlData::GetDescription()
{
	return description;
}

void SpyControlData::SetType(DWORD value)
{
	type = value;
}

DWORD SpyControlData::GetType()
{
	return type;
}

void SpyControlData::SetRect(LPRECT value)
{
	CopyMemory(&rect, value, sizeof(RECT));
}

LPRECT SpyControlData::GetRect()
{
	return &rect;
}

void SpyControlData::SetClassname(string value)
{
	className = value;
}

string SpyControlData::GetClassname()
{
	return className;
}

void SpyControlData::SetAutoID(string value)
{
	autoID = value;
}

string SpyControlData::GetAutoID()
{
	return autoID;
}

void SpyControlData::SetValue(string value)
{
	controlValue = value;
}

string SpyControlData::GetValue()
{
	return controlValue;
}

void SpyControlData::SetScreenControlType(string value)
{
	screenControlType = value;
}

string SpyControlData::GetScreenControlType()
{
	return screenControlType;
}


void SpyControlData::SetClickAnnotation(string value)
{
	strClickAnnotation = value;
}

string SpyControlData::GetClickAnnotation()
{
	return strClickAnnotation;
}

void SpyControlData::SetEnterAnnotation(string value)
{
	strEnterAnnotation = value;
}

string SpyControlData::GetEnterAnnotation()
{
	return strEnterAnnotation;
}

string SpyControlData::GetControlRect()
{
	char str[100];

	sprintf_s(str, "L=%d, T=%d, R=%d, B=%d", rect.left, rect.top, rect.right, rect.bottom);
	string strRect = str;
	return strRect;
}

string SpyControlData::GetControlTypeName()
{
	string strName = "";

	switch (type)
	{
	case UIA_ButtonControlTypeId:
		strName = "Button";
		break;
	case UIA_CalendarControlTypeId:
		strName = "Calendar";
		break;
	case UIA_CheckBoxControlTypeId:
		strName = "CheckBox";
		break;
	case UIA_ComboBoxControlTypeId:
		strName = "ComboBox";
		break;
	case UIA_EditControlTypeId:
		strName = "Edit";
		break;
	case UIA_HyperlinkControlTypeId:
		strName = "Hyperlink";
		break;
	case UIA_ImageControlTypeId:
		strName = "Image";
		break;
	case UIA_ListItemControlTypeId:
		strName = "ListItem";
		break;
	case UIA_ListControlTypeId:
		strName = "List";
		break;
	case UIA_MenuControlTypeId:
		strName = "Menu";
		break;
	case UIA_MenuBarControlTypeId:
		strName = "MenuBar";
		break;
	case UIA_MenuItemControlTypeId:
		strName = "MenuItem";
		break;
	case UIA_ProgressBarControlTypeId:
		strName = "ProgressBar";
		break;
	case UIA_RadioButtonControlTypeId:
		strName = "RadioButton";
		break;
	case UIA_ScrollBarControlTypeId:
		strName = "ScrollBar";
		break;
	case UIA_SliderControlTypeId:
		strName = "Slider";
		break;
	case UIA_SpinnerControlTypeId:
		strName = "Spinner";
		break;
	case UIA_StatusBarControlTypeId:
		strName = "StatusBar";
		break;
	case UIA_TabControlTypeId:
		strName = "Tab";
		break;
	case UIA_TabItemControlTypeId:
		strName = "TabItem";
		break;
	case UIA_TextControlTypeId:
		strName = "Text";
		break;
	case UIA_ToolBarControlTypeId:
		strName = "ToolBar";
		break;
	case UIA_ToolTipControlTypeId:
		strName = "ToolTip";
		break;
	case UIA_TreeControlTypeId:
		strName = "Tree";
		break;
	case UIA_TreeItemControlTypeId:
		strName = "TreeItem";
		break;
	case UIA_CustomControlTypeId:
		strName = "Custom";
		break;
	case UIA_GroupControlTypeId:
		strName = "Group";
		break;
	case UIA_ThumbControlTypeId:
		strName = "Thumb";
		break;
	case UIA_DataGridControlTypeId:
		strName = "DataGrid";
		break;
	case UIA_DataItemControlTypeId:
		strName = "DataItem";
		break;
	case UIA_DocumentControlTypeId:
		strName = "Document";
		break;
	case UIA_SplitButtonControlTypeId:
		strName = "SplitButton";
		break;
	case UIA_WindowControlTypeId:
		strName = "Windows";
		break;
	case UIA_PaneControlTypeId:
		strName = "Pane";
		break;
	case UIA_HeaderControlTypeId:
		strName = "Header";
		break;
	case UIA_HeaderItemControlTypeId:
		strName = "HeaderItem";
		break;
	case UIA_TableControlTypeId:
		strName = "Table";
		break;
	case UIA_TitleBarControlTypeId:
		strName = "TitleBar";
		break;
	case UIA_SeparatorControlTypeId:
		strName = "Separator";
		break;
	case UIA_SemanticZoomControlTypeId:
		strName = "SemanticZoom";
		break;
	case UIA_AppBarControlTypeId:
		strName = "AppBar";
		break;
	default:
		break;
	}

	return strName;
}


void SpyControlData::ClearMatchData()
{
	vector <struct matchResult *>::iterator itMatch = matchResults.begin();
	while (itMatch != matchResults.end())
	{
		struct matchResult *pMatchResult = *itMatch;
		delete pMatchResult;

		itMatch++;
	}
	matchResults.clear();

}

int SpyControlData::GetMatchCount()
{
	return matchResults.size();
}

void SpyControlData::AddMatchData(bool bPrimary, bool bRect, bool bName, bool bValue, WindowControlData *pControl)
{
	struct matchResult *pMatchResult = new struct matchResult;
	pMatchResult->primaryMatch = bPrimary;
	pMatchResult->rectMatch = bRect;
	pMatchResult->nameMatch = bName;
	pMatchResult->valueMatch = bValue;
	pMatchResult->spyControlData = this;
	pMatchResult->wndControlData = pControl;
	matchResults.push_back(pMatchResult);
}

void SpyControlData::DeleteMatchData(WindowControlData *pControl)
{
	vector <struct matchResult *>::iterator itMatch = matchResults.begin();
	struct matchResult *pMatchResult = NULL;
	while (itMatch != matchResults.end())
	{
		pMatchResult = *itMatch;
		if (pMatchResult->wndControlData == pControl)
			break;
		itMatch++;
	}
	if (pMatchResult != NULL)
	{
		delete pMatchResult;
		matchResults.erase(itMatch);
	}
}

struct matchResult *SpyControlData::GetMatchData(int num)
{
	return matchResults[num];
}


void SpyControlData::SetParentFilePosition(long long value)
{
	fPosParent = value;
}

long long SpyControlData::GetParentFilePosition()
{
	return fPosParent;
}


void SpyControlData::SetLeftSiblingFilePosition(long long value)
{
	fPosLeftSibling = value;
}

long long SpyControlData::GetLeftSiblingFilePosition()
{
	return fPosLeftSibling;
}


void SpyControlData::SetRightSiblingFilePosition(long long value)
{
	fPosRightSibling = value;
}

long long SpyControlData::GetRightSiblingFilePosition()
{
	return fPosRightSibling;
}
