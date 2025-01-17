#include "stdafx.h"
#include "CaptureElementData.h"
#include <UIAutomationClient.h>

CaptureElementData::CaptureElementData()
{
	captureType = 0;

}

void CaptureElementData::SetCaptureType(int value)
{
	captureType = value;
}

int CaptureElementData::GetCaptureType()
{
	return captureType;
}

CaptureElementData::~CaptureElementData()
{
	list <ElementData*>::iterator itList = parents.begin();
	while (itList != parents.end())
	{
		ElementData* ptr = *itList;
		delete ptr;
		itList++;
	}

	itList = leftSiblings.begin();
	while (itList != leftSiblings.end())
	{
		ElementData* ptr = *itList;
		delete ptr;
		itList++;
	}

	itList = rightSiblings.begin();
	while (itList != rightSiblings.end())
	{
		ElementData* ptr = *itList;
		delete ptr;
		itList++;
	}

}

ElementData* CaptureElementData::GetElementData()
{
	return &data;
}

string CaptureElementData::GetProcessName()
{
	return data.GetProcessName();
}

string CaptureElementData::GetAutomationID()
{
	return data.GetAutomationID();
}

void CaptureElementData::GetBoundingRect(LPRECT rect)
{
	data.GetBoundingRect(rect);
}

string CaptureElementData::GetControlClassName()
{
	return data.GetControlClassName();;
}

string CaptureElementData::GetItemType()
{
	return data.GetItemType();
}

string CaptureElementData::GetControlName()
{
	return data.GetControlName();;
}

void CaptureElementData::SetAnnotation(string value)
{
	annotation = value;
}

void CaptureElementData::SetUserDefinedName(string value)
{
	userName = value;
}

string CaptureElementData::GetAnnotation()
{
	return annotation;
}

string CaptureElementData::GetUserDefinedName()
{
	return userName;
}


string CaptureElementData::GetControlTypeName()
{
	string strName = "";

	switch (data.GetControlType())
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



CONTROLTYPEID CaptureElementData::GetControlType()
{
	return data.GetControlType();;
}

void CaptureElementData::SetProcessName(string value)
{
	data.SetProcessName(value);
}

void CaptureElementData::SetAutomationID(string value)
{
	data.SetAutomationID(value);
}

void CaptureElementData::SetBoundingRect(LPRECT rect)
{
	data.SetBoundingRect(rect);
}

void CaptureElementData::SetClassName(string value)
{
	data.SetClassName(value);
}

void CaptureElementData::SetItemType(string value)
{
	data.SetItemType(value);
}

void CaptureElementData::SetControlName(string value)
{
	data.SetControlName(value);
}

void CaptureElementData::SetControlType(CONTROLTYPEID value)
{
	data.SetControlType(value);
}

list <ElementData*>* CaptureElementData::GetParentList()
{
	return &parents;
}

list <ElementData*>* CaptureElementData::GetLeftSiblingList()
{
	return &leftSiblings;
}

list <ElementData*>* CaptureElementData::GetRightSiblingList()
{
	return &rightSiblings;
}


string CaptureElementData::GetWindowName()
{
	return windowName;
}

void CaptureElementData::SetWindowName(string value)
{
	windowName = value;
}