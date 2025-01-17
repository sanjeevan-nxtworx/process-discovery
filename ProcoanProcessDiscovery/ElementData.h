#pragma once
#include <string>
#include <UIAutomationCore.h>
using namespace std;

class ElementData
{
	string processName;
	string automationID;
	RECT boundingRect;
	string className;
	CONTROLTYPEID controlType;
	string itemType;
	string controlName; // Unique name for the control
public:
	ElementData();
	~ElementData();

	string GetProcessName();
	string GetAutomationID();
	void GetBoundingRect(LPRECT rect);
	CONTROLTYPEID GetControlType();
	string GetControlClassName();
	string GetItemType();
	string GetControlName();

	void SetProcessName(string value);
	void SetAutomationID(string value);
	void SetBoundingRect(LPRECT rect);
	void SetControlType(CONTROLTYPEID value);
	void SetClassName(string value);
	void SetItemType(string value);
	void SetControlName(string value);
};

