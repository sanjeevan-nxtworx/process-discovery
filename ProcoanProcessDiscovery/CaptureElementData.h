#pragma once
#include "ElementData.h"
#include <list>

using namespace std;

class CaptureElementData
{
	int captureType;
	ElementData data;
	list <ElementData *> parents;
	list <ElementData *> leftSiblings;
	list <ElementData *> rightSiblings;
	string windowName;
	string annotation;
	string userName;

public:
	CaptureElementData();
	~CaptureElementData();

	void SetCaptureType(int value);
	int GetCaptureType();
	ElementData* GetElementData();
	string GetWindowName();
	void SetWindowName(string value);
	string GetProcessName();
	string GetAutomationID();
	void GetBoundingRect(LPRECT rect);
	CONTROLTYPEID GetControlType();
	string GetControlClassName();
	string GetItemType();
	string GetControlName();
	string GetControlTypeName();

	void SetAnnotation(string value);
	void SetUserDefinedName(string value);
	string GetAnnotation();
	string GetUserDefinedName();


	void SetProcessName(string value);
	void SetAutomationID(string value);
	void SetBoundingRect(LPRECT rect);
	void SetControlType(CONTROLTYPEID value);
	void SetClassName(string value);
	void SetItemType(string value);
	void SetControlName(string value);
	list <ElementData*>* GetParentList();
	list <ElementData*>* GetLeftSiblingList();
	list <ElementData*>* GetRightSiblingList();


};

