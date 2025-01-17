#pragma once
#include <string>
#include <vector>
#include "SpyHeader.h"

using namespace std;

class SpyControlData;
class SpyControlProxy;

class WindowControlData
{
	DWORD controlID;  // This is the ID of the control within the Layout
	DWORD ID;		  // This is the database ID
	string name;
	DWORD type;
	RECT rect;
	string className;
	string autoID;
	string controlValue;

	vector <WindowControlData *> controlData;
	vector <struct matchResult *> matchResults;
	struct matchResult matchValue;

	bool bMatch;

	WindowControlData *parentData;
	vector <string> ancestorsKey;
	vector <string> leftSiblingKey;
	vector <string> rightSiblingKey;
	vector <string> childrenKey;
	vector <string> neighbourKey;


public:
	WindowControlData();
	~WindowControlData();

	void SetParent(WindowControlData *value);
	WindowControlData *GetParent();
	void SetAncestorKey(vector <string> &value);
	void SetLeftSiblingKey(vector <string> &value);
	void SetRightSiblingKey(vector <string> &value);
	void SetChildrenKey(vector <string> &value);
	void CreateNeighborKey();

	vector <string> &GetAncestorKey();
	vector <string> &GetLeftSiblingKey();
	vector <string> &GetRightSiblingKey();
	vector <string> &GetChildrenKey();
	vector <string> &GetNeighbourKey();

	void SetID(DWORD value);
	DWORD GetID();

	void SetControlID(DWORD value);
	DWORD GetControlID();

	void SetName(string value);
	string GetName();

	void SetMatch(bool value);
	bool GetMatch();


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

	vector <WindowControlData *> *GetChildControls();

	void ClearMatchData();
	void AddMatchData(bool bPrimary, bool bRect, bool bName, bool bValue, SpyControlData *pControl);
	void MatchWithProxy(SpyControlProxy *pControlProxy, SpyControlData *pControl);
	bool PerformMatchWithProxy(SpyControlProxy *pControlProxy, SpyControlData *pControl, struct matchResult *pMatchResult);
	bool IsMatchWithProxy(SpyControlProxy *pControlProxy, SpyControlData *pControl);
	void DeleteMatchData(SpyControlData *pControl);
	int GetMatchCount();
	struct matchResult *GetMatchData(int num);
	void SetMatchedValue(struct matchResult *pMatchResult);
	void CheckMatchedValue(struct matchResult *pMatchResult, DWORD ID);
};

