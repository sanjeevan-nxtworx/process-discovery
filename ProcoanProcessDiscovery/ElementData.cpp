#include "stdafx.h"
#include "ElementData.h"

ElementData::ElementData()
{
	processName = "";
	automationID = "";
	ZeroMemory(&boundingRect, sizeof(RECT));
	className = "";
	controlType = 0;
	itemType = "";
	controlName = "";
}

ElementData::~ElementData()
{

}

string ElementData::GetProcessName()
{
	return processName;
}

string ElementData::GetAutomationID()
{
	return automationID;
}

void ElementData::GetBoundingRect(LPRECT rect)
{
	CopyMemory(rect, &boundingRect, sizeof(RECT));
}

string ElementData::GetControlClassName()
{
	return className;
}

string ElementData::GetItemType()
{
	return itemType;
}

string ElementData::GetControlName()
{
	return controlName;
}

CONTROLTYPEID ElementData::GetControlType()
{
	return controlType;
}

void ElementData::SetProcessName(string value)
{
	processName = value;
}

void ElementData::SetAutomationID(string value)
{
	automationID = value;
}

void ElementData::SetBoundingRect(LPRECT rect)
{
	CopyMemory(&boundingRect, rect, sizeof(RECT));
}

void ElementData::SetClassName(string value)
{
	className = value;
}

void ElementData::SetItemType(string value)
{
	itemType = value;
}

void ElementData::SetControlName(string value)
{
	controlName = value;
}

void ElementData::SetControlType(CONTROLTYPEID value)
{
	controlType = value;
}
