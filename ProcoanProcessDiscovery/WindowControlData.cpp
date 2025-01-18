#include "stdafx.h"
#include "WindowControlData.h"
#include "SpyControlData.h"
#include "SpyHeader.h"
#include "SpyControlProxy.h"

WindowControlData::WindowControlData()
{
	name = "";
	type = 0L;
	ZeroMemory(&rect, sizeof(RECT));
	className = "";
	autoID = "";
	controlValue = "";
	controlData.clear();
	bMatch = false;
	matchResults.clear();
	ID = controlID = 0L;
	ZeroMemory(&matchValue, sizeof(struct matchResult));
}


WindowControlData::~WindowControlData()
{
	vector <WindowControlData *>::iterator itControls = controlData.begin();
	while (itControls != controlData.end())
	{
		WindowControlData *pWindowControls = *itControls;
		delete pWindowControls;
		itControls++;
	}
	controlData.clear();
	ClearMatchData();
}


void WindowControlData::SetID(DWORD value)
{
	ID = value;
}

DWORD WindowControlData::GetID()
{
	return ID;
}

void WindowControlData::SetParent(WindowControlData *value)
{
	parentData = value;
}


void WindowControlData::SetAncestorKey(vector <string> &value)
{
	ancestorsKey = value;
}

void WindowControlData::SetLeftSiblingKey(vector <string> &value)
{
	leftSiblingKey = value;
}

void WindowControlData::SetRightSiblingKey(vector <string> &value)
{
	rightSiblingKey = value;
}

void WindowControlData::CreateNeighborKey()
{
	neighbourKey.clear();
	if (ancestorsKey.size() > 0)
	{
		string key = ancestorsKey.front();
		neighbourKey.push_back(key);
	}
	else
		neighbourKey.push_back("");

	if (leftSiblingKey.size() > 0)
	{
		string key = leftSiblingKey.back();
		neighbourKey.push_back(key);
	}
	else
		neighbourKey.push_back("");

	if (rightSiblingKey.size() > 0)
	{
		string key = rightSiblingKey.front();
		neighbourKey.push_back(key);
	}
	else
		neighbourKey.push_back("");

}

void WindowControlData::SetChildrenKey(vector <string> &value)
{
	childrenKey = value;
}

vector <string> &WindowControlData::GetAncestorKey()
{
	return ancestorsKey;
}

vector <string> &WindowControlData::GetLeftSiblingKey()
{
	return leftSiblingKey;
}

vector <string> &WindowControlData::GetRightSiblingKey()
{
	return rightSiblingKey;
}

vector <string> &WindowControlData::GetChildrenKey()
{
	return childrenKey;
}

vector <string> &WindowControlData::GetNeighbourKey()
{
	return neighbourKey;
}


WindowControlData *WindowControlData::GetParent()
{
	return parentData;
}

void WindowControlData::SetControlID(DWORD value)
{
	controlID = value;
}

DWORD WindowControlData::GetControlID()
{
	return controlID;
}

void WindowControlData::SetMatch(bool value)
{
	bMatch = value;
}

bool WindowControlData::GetMatch()
{
	return bMatch;
}

void WindowControlData::SetName(string value)
{
	name = value;
}

string WindowControlData::GetName()
{
	return name;
}

vector <WindowControlData *> *WindowControlData::GetChildControls()
{
	return &controlData;
}


void WindowControlData::SetType(DWORD value)
{
	type = value;
}

DWORD WindowControlData::GetType()
{
	return type;
}

void WindowControlData::SetRect(LPRECT value)
{
	CopyMemory(&rect, value, sizeof(RECT));
}

LPRECT WindowControlData::GetRect()
{
	return &rect;
}

void WindowControlData::SetClassname(string value)
{
	className = value;
}

string WindowControlData::GetClassname()
{
	return className;
}

void WindowControlData::SetAutoID(string value)
{
	autoID = value;
}

string WindowControlData::GetAutoID()
{
	return autoID;
}

void WindowControlData::SetValue(string value)
{
	controlValue = value;
}

string WindowControlData::GetValue()
{
	return controlValue;
}


void WindowControlData::ClearMatchData()
{
	vector <struct matchResult *>::iterator itMatch = matchResults.begin();
	while (itMatch != matchResults.end())
	{
		struct matchResult *pMatchResult = *itMatch;
		delete pMatchResult;

		itMatch++;
	}
	matchResults.clear();
	SetMatch(false);

}

bool WindowControlData::PerformMatchWithProxy(SpyControlProxy *pControlProxy, SpyControlData *pControl, struct matchResult *pMatchResult)
{
	pMatchResult->primaryMatch = false;
	pMatchResult->rectMatch = false;
	pMatchResult->nameMatch = false;
	pMatchResult->valueMatch = false;
	pMatchResult->matchAncestors = NULL;
	pMatchResult->matchLeftSibling = NULL;
	pMatchResult->matchRightSibling = NULL;
	pMatchResult->matchChildren = NULL;
	pMatchResult->matchNeighbors = NULL;
	pMatchResult->wndControlData = NULL;
	pMatchResult->spyControlData = NULL;


	if (pControl->GetName() == GetName() && (GetName() == "Personal Info" || GetName() == "Save"))
	{
		pMatchResult->primaryMatch = false;
	}

	//if (!pControlProxy->AncestorKeyValid(ancestorsKey) || !pControlProxy->LeftSiblingKeyValid(leftSiblingKey) || 
	//	!pControlProxy->RightSiblingKeyValid(rightSiblingKey) || !pControlProxy->ChildrenKeyValid(childrenKey))
	//{
	//	return;
	//}


	pMatchResult->matchAncestors = pControlProxy->MatchAncestorKey(ancestorsKey);
	pMatchResult->matchLeftSibling = pControlProxy->MatchLeftSiblingKey(leftSiblingKey);
	pMatchResult->matchRightSibling = pControlProxy->MatchRightSiblingKey(rightSiblingKey);
	pMatchResult->matchChildren = pControlProxy->MatchChildrenKey(childrenKey);
//	pMatchResult->matchNeighbors = pControlProxy->MatchNeighbourKey(neighbourKey);
	bool bFullyMatched = pMatchResult->matchAncestors == 100 &&
							pMatchResult->matchLeftSibling == 100 &&
							pMatchResult->matchRightSibling == 100 &&
							pMatchResult->matchChildren == 100  //&&
						   //		pMatchResult->matchNeighbors == 100
							;
	bool bNotMatched = pMatchResult->matchAncestors == 0 ||
						pMatchResult->matchLeftSibling == 0 ||
						pMatchResult->matchRightSibling == 0 ||
						pMatchResult->matchChildren == 0  //||
					   //		pMatchResult->matchNeighbors == 0
						;

	if (bNotMatched)
		;
	else
	{
		// Check Primary Compare criteria
		if (pControl->GetAutoID() == GetAutoID() &&
			pControl->GetType() == GetType() &&
			pControl->GetClassname() == GetClassname()
			)
		{
			bool bName = false;
			bool bValue = false;
			bool bRect = false;
			LPRECT bRect1, bRect2;
			bRect1 = pControl->GetRect();
			bRect2 = GetRect();
			if (bRect1->top == bRect2->top && bRect1->bottom == bRect2->bottom && bRect1->right == bRect2->right && bRect1->left == bRect2->left)
			{
				bRect = true;
			}
			if (pControl->GetName() == GetName())
				bName = true;
			if (pControl->GetValue() == GetValue())
				bValue = true;

			pMatchResult->primaryMatch = bName;
			pMatchResult->rectMatch = bRect;
			pMatchResult->nameMatch = bName;
			pMatchResult->valueMatch = bValue;
			pMatchResult->spyControlData = pControl;
			pMatchResult->wndControlData = this;
			if (bName)
			{
				if (bFullyMatched || GetName() != "")
				{
					SetMatch(bName);
					SetControlID(pControl->GetControlID());
					SetMatchedValue(pMatchResult);
					return true;
				}
				else
				{
					
					CheckMatchedValue(pMatchResult, pControl->GetControlID());
				}
			}
		}
	}
	return false;
}


void WindowControlData::SetMatchedValue(struct matchResult* pMatchResult)
{
	CopyMemory(&matchValue, pMatchResult, sizeof(struct matchResult));
}

void WindowControlData::CheckMatchedValue(struct matchResult* pMatchResult, DWORD id)
{
	if (pMatchResult->matchAncestors >= matchValue.matchAncestors &&
		pMatchResult->matchChildren >= matchValue.matchChildren &&
		pMatchResult->matchLeftSibling >= matchValue.matchLeftSibling &&
		pMatchResult->matchRightSibling >= matchValue.matchRightSibling &&
		pMatchResult->matchNeighbors >= matchValue.matchNeighbors)
	{
		CopyMemory(&matchValue, pMatchResult, sizeof(struct matchResult));
		controlID = id;
		bMatch = true;
	}
}

bool WindowControlData::IsMatchWithProxy(SpyControlProxy *pControlProxy, SpyControlData *pControl)
{
	struct matchResult matchResult;
	return PerformMatchWithProxy(pControlProxy, pControl, &matchResult);
}



void WindowControlData::MatchWithProxy(SpyControlProxy *pControlProxy, SpyControlData *pControl)
{
	IsMatchWithProxy(pControlProxy, pControl);
}

void WindowControlData::AddMatchData(bool bPrimary, bool bRect, bool bName, bool bValue, SpyControlData *pControl)
{
	struct matchResult *pMatchResult = new struct matchResult;
	ZeroMemory(pMatchResult, sizeof(struct matchResult));
	pMatchResult->primaryMatch = bPrimary;
	pMatchResult->rectMatch = bRect;
	pMatchResult->nameMatch = bName;
	pMatchResult->valueMatch = bValue;
	pMatchResult->spyControlData = pControl;
	pMatchResult->wndControlData = this;
	matchResults.push_back(pMatchResult);
}

void WindowControlData::DeleteMatchData(SpyControlData *pControl)
{
	vector <struct matchResult *>::iterator itMatch = matchResults.begin();
	struct matchResult *pMatchResult = NULL;
	while (itMatch != matchResults.end())
	{
		pMatchResult = *itMatch;
		if (pMatchResult->spyControlData == pControl)
			break;
		itMatch++;
	}
	if (pMatchResult != NULL)
	{
		delete pMatchResult;
		matchResults.erase(itMatch);
	}
}

struct matchResult *WindowControlData::GetMatchData(int num)
{
	return matchResults[num];
}

int WindowControlData::GetMatchCount()
{
	return (int)matchResults.size();
}

