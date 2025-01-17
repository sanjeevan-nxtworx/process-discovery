#pragma once
#include <vector>
#include <string>

using namespace std;

class SpyControlProxy
{
	DWORD controlID;
	SpyControlProxy *parentData;
	vector <SpyControlProxy *> childData;
	vector <string> ancestorsKey;
	vector <string> leftSiblingKey;
	vector <string> rightSiblingKey;
	vector <string> childrenKey;
	vector <string> neighbourKey;
	DWORD controlType;

public:
	SpyControlProxy(DWORD id);
	void SetType(DWORD value);
	DWORD GetType();
	void SetParent(SpyControlProxy *value);
	void SetAncestorKey(vector <string> &value);
	void SetLeftSiblingKey(vector <string> &value);
	void SetRightSiblingKey(vector <string> &value);
	void SetChildrenKey(vector <string> &value);
	void CreateNeighborKey();
	int MatchKeys(vector <string> &leftKey, vector <string> &proxyKey);
	bool IsAnyKeyInKey(vector <string>& key1, vector <string>& key2);

	SpyControlProxy *GetParent();
	SpyControlProxy *AddChildControl(DWORD id);
	~SpyControlProxy();
	SpyControlProxy *GetControl(DWORD id);
	DWORD GetControlID();
	vector <SpyControlProxy *> *GetChildControls();
	int MatchAncestorKey(vector <string> &value);
	int MatchLeftSiblingKey(vector <string> &value);
	int MatchRightSiblingKey(vector <string> &value);
	int MatchChildrenKey(vector <string> &value);
	int MatchNeighbourKey(vector <string> &value);
	int MatchType(DWORD value);
	bool AncestorKeyValid(vector <string>& value);
	bool LeftSiblingKeyValid(vector <string>& value);
	bool RightSiblingKeyValid(vector <string>& value);
	bool ChildrenKeyValid(vector <string>& value);


};

